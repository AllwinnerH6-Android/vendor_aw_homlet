/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <binder/IServiceManager.h>
#include "HWCProxy.h"

namespace android {

ANDROID_SINGLETON_STATIC_INSTANCE(HWCProxy);

HWCProxy::HWCProxy() : Singleton<HWCProxy>()
{
    std::unique_lock<std::mutex> lock(mLock);
    connect();
}

void HWCProxy::connect()
{
    const String16 name("HWCPrivateService");
    while (getService(name, &mHwcService) != NO_ERROR) {
        usleep(250000);
    }
    assert(mHwcService != NULL);

    /* Create the death listener. */
    class DeathObserver : public IBinder::DeathRecipient {
        HWCProxy& mService;
        virtual void binderDied(const wp<IBinder>& who) {
            ALOGW("HWCProxy remote (HWCPrivateService) died [%p]",
                  who.unsafe_get());
            mService.hwcServiceDied();
        }
    public:
        explicit DeathObserver(HWCProxy& s) : mService(s) {}
    };

    mDeathObserver = new DeathObserver(*const_cast<HWCProxy*>(this));
    IInterface::asBinder(mHwcService)->linkToDeath(mDeathObserver);
}


/* static */ sp<IHWCPrivateService> HWCProxy::getHwcService()
{
    HWCProxy& instance = HWCProxy::getInstance();
    std::unique_lock<std::mutex> lock(instance.mLock);
    if (instance.mHwcService == NULL) {
        HWCProxy::getInstance().connect();
        assert(instance.mHwcService != NULL);
        ALOGD("HWCProxy reconnected");
    }
    return instance.mHwcService;
}

/* static */ void HWCProxy::registerDeathNotify(const HWCProxy::Callback *cb)
{
    HWCProxy& instance = HWCProxy::getInstance();
    std::unique_lock<std::mutex> lock(instance.mLock);
    instance.mCallback = cb;
}

void HWCProxy::hwcServiceDied()
{
    std::unique_lock<std::mutex> lock(mLock);
    mHwcService = NULL;
    mDeathObserver = NULL;

    if (mCallback)
        mCallback->deathNotify(mCallback);
}

};

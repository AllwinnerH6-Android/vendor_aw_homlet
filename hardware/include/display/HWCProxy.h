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

#ifndef DISPLAYD_HWC_SERVICE_H_
#define DISPLAYD_HWC_SERVICE_H_

#include <stdint.h>
#include <sys/types.h>
#include <mutex>

#include <utils/Singleton.h>
#include <utils/StrongPointer.h>
#include "IHWCPrivateService.h"

namespace android {

class HWCProxy : public Singleton<HWCProxy>
{
public:
    struct Callback {
        void (*deathNotify)(const Callback *cb);
    };

    static sp<IHWCPrivateService> getHwcService();
    static void registerDeathNotify(const Callback *cb);

private:
    Callback const *mCallback;
    sp<IHWCPrivateService> mHwcService;
    sp<IBinder::DeathRecipient> mDeathObserver;
    std::mutex mLock;

    HWCProxy();
    void connect();
    void hwcServiceDied();
    friend class Singleton<HWCProxy>;
};

};

#endif

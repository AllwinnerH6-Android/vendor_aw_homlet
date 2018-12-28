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

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>

#include "IDisplaydCallback.h"

namespace android {

enum {
    CALLBACK_DATASPACE_CHANGE = IBinder::FIRST_CALL_TRANSACTION + 0,
    CALLBACK_RESERVE,
};

class BpDisplaydCallback : public BpInterface<IDisplaydCallback> {
public:
    BpDisplaydCallback(const sp<IBinder>& impl)
      : BpInterface<IDisplaydCallback>(impl) {}

    virtual int dataspaceChange(int m) {
        Parcel data, reply;
        data.writeInterfaceToken(IDisplaydCallback::getInterfaceDescriptor());
        data.writeInt32(m);
        remote()->transact(CALLBACK_DATASPACE_CHANGE, data, &reply);
        return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(DisplaydCallback, "hwc.adapter.callback");

status_t BnDisplaydCallback::onTransact(uint32_t code,
            const Parcel& data, Parcel* reply, uint32_t flags) {

    switch (code) {
    case CALLBACK_DATASPACE_CHANGE: {
        CHECK_INTERFACE(IDisplaydCallback, data, reply);
        int m = data.readInt32();
        int ret = dataspaceChange(m);
        reply->writeInt32(ret);
        return NO_ERROR;
    }
    default:
        return BBinder::onTransact(code, data, reply, flags);
    }
}

};


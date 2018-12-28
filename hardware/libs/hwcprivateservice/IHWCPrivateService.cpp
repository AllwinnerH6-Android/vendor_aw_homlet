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

#include "IHWCPrivateService.h"

namespace android {

enum {
    HWC_SWITCH_DEVICE = IBinder::FIRST_CALL_TRANSACTION + 0,
    HWC_BLANK_DISPLAY,
    HWC_SET_OUTPUT_MODE,

    HWC_SET_MARGIN,
    HWC_SET_VIDEO_RATIO,
    HWC_SET_3D_MODE,
    HWC_SET_DATASPACE,
    HWC_REGISTER_CALLBACK,

    HWC_SET_DEBUG_TAG,
};

class BpHWCPrivateService : public BpInterface<IHWCPrivateService> {
public:
    BpHWCPrivateService(const sp<IBinder>& impl)
      : BpInterface<IHWCPrivateService>(impl) {}

    virtual int switchDevice(const DeviceTable& tables) {
        Parcel data, reply;
        data.writeInterfaceToken(IHWCPrivateService::getInterfaceDescriptor());
        data.write(tables);
        remote()->transact(HWC_SWITCH_DEVICE, data, &reply);
        return reply.readInt32();
    }

    virtual int blank(int display, int enable) {
        Parcel data, reply;
        data.writeInterfaceToken(IHWCPrivateService::getInterfaceDescriptor());
        data.writeInt32(display);
        data.writeInt32(enable);
        remote()->transact(HWC_BLANK_DISPLAY, data, &reply);
        return reply.readInt32();
    }

    virtual int setOutputMode(int display, int type, int mode) {
        Parcel data, reply;
        data.writeInterfaceToken(IHWCPrivateService::getInterfaceDescriptor());
        data.writeInt32(display);
        data.writeInt32(type);
        data.writeInt32(mode);
        remote()->transact(HWC_SET_OUTPUT_MODE, data, &reply);
        return reply.readInt32();
    }

    virtual int setMargin(int display, int l, int r, int t, int b) {
        Parcel data, reply;
        data.writeInterfaceToken(IHWCPrivateService::getInterfaceDescriptor());
        data.writeInt32(display);
        data.writeInt32(l);
        data.writeInt32(r);
        data.writeInt32(t);
        data.writeInt32(b);
        remote()->transact(HWC_SET_MARGIN, data, &reply);
        return reply.readInt32();
    }

    virtual int setVideoRatio(int display, int ratio) {
        Parcel data, reply;
        data.writeInterfaceToken(IHWCPrivateService::getInterfaceDescriptor());
        data.writeInt32(display);
        data.writeInt32(ratio);
        remote()->transact(HWC_SET_VIDEO_RATIO, data, &reply);
        return reply.readInt32();
    }

    virtual int set3DMode(int display, int mode) {
        Parcel data, reply;
        data.writeInterfaceToken(IHWCPrivateService::getInterfaceDescriptor());
        data.writeInt32(display);
        data.writeInt32(mode);
        remote()->transact(HWC_SET_3D_MODE, data, &reply);
        return reply.readInt32();
    }

    virtual int setDataspace(int display, int dataspace) {
        Parcel data, reply;
        data.writeInterfaceToken(IHWCPrivateService::getInterfaceDescriptor());
        data.writeInt32(display);
        data.writeInt32(dataspace);
        remote()->transact(HWC_SET_DATASPACE, data, &reply);
        return reply.readInt32();
    }

    virtual void setDebugTag(int32_t tag) {
        Parcel data, reply;
        data.writeInterfaceToken(IHWCPrivateService::getInterfaceDescriptor());
        data.writeInt32(tag);
        remote()->transact(HWC_SET_DEBUG_TAG, data, &reply);
        return;
    }

    virtual int registerCallback(sp<IBinder>& binder) {
        Parcel data, reply;
        data.writeInterfaceToken(IHWCPrivateService::getInterfaceDescriptor());
        data.writeStrongBinder(binder);
        remote()->transact(HWC_REGISTER_CALLBACK, data, &reply);
        return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(HWCPrivateService, "com.allwinnertech.hwcservice");

status_t BnHWCPrivateService::onTransact(uint32_t code,
            const Parcel& data, Parcel* reply, uint32_t flags) {

    switch (code) {
    case HWC_SWITCH_DEVICE: {
        CHECK_INTERFACE(IHWCPrivateService, data, reply);
        DeviceTable tables;
        data.read(tables);
        int ret = switchDevice(tables);
        reply->writeInt32(ret);
        return NO_ERROR;
    }
    case HWC_BLANK_DISPLAY: {
        CHECK_INTERFACE(IHWCPrivateService, data, reply);
        int display = data.readInt32();
        int enable = data.readInt32();
        int ret = blank(display, enable);
        reply->writeInt32(ret);
        return NO_ERROR;
    }
    case HWC_SET_OUTPUT_MODE: {
        CHECK_INTERFACE(IHWCPrivateService, data, reply);
        int display = data.readInt32();
        int type = data.readInt32();
        int mode = data.readInt32();
        int ret = setOutputMode(display, type, mode);
        reply->writeInt32(ret);
        return NO_ERROR;
    }
    case HWC_SET_MARGIN: {
        CHECK_INTERFACE(IHWCPrivateService, data, reply);
        int display = data.readInt32();
        int l = data.readInt32();
        int r = data.readInt32();
        int t = data.readInt32();
        int b = data.readInt32();
        int ret = setMargin(display, l, r, t, b);
        reply->writeInt32(ret);
        return NO_ERROR;
    }
    case HWC_SET_VIDEO_RATIO: {
        CHECK_INTERFACE(IHWCPrivateService, data, reply);
        int display = data.readInt32();
        int ratio = data.readInt32();
        int ret = setVideoRatio(display, ratio);
        reply->writeInt32(ret);
        return NO_ERROR;
    }
    case HWC_SET_3D_MODE: {
        CHECK_INTERFACE(IHWCPrivateService, data, reply);
        int display = data.readInt32();
        int mode = data.readInt32();
        int ret = set3DMode(display, mode);
        reply->writeInt32(ret);
        return NO_ERROR;
    }
    case HWC_SET_DATASPACE: {
        CHECK_INTERFACE(IHWCPrivateService, data, reply);
        int display = data.readInt32();
        int dataspace = data.readInt32();
        int ret = setDataspace(display, dataspace);
        reply->writeInt32(ret);
        return NO_ERROR;
    }
    case HWC_REGISTER_CALLBACK: {
        CHECK_INTERFACE(IHWCPrivateService, data, reply);
        sp<IBinder> cb = data.readStrongBinder();
        int ret = registerCallback(cb);
        reply->writeInt32(ret);
        return NO_ERROR;
    }
    case HWC_SET_DEBUG_TAG: {
        CHECK_INTERFACE(IHWCPrivateService, data, reply);
        int tag = data.readInt32();
        setDebugTag(tag);
        reply->writeInt32(0);
        return NO_ERROR;
    }
    default:
        return BBinder::onTransact(code, data, reply, flags);
    }
}

};


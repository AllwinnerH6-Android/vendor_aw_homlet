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

#ifndef __IHWC_PRIVATE_SERVICE_H__
#define __IHWC_PRIVATE_SERVICE_H__

#include <stdint.h>
#include <sys/types.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include "DeviceTable.h"

namespace android {

class IHWCPrivateService : public IInterface
{
public:

    /* Sync wiht device/device_hdmi.h define! */
    enum _dataspace_mode {
        eDataspaceSdr = 0x1001,
        eDataspaceHdr = 0x1002,
    };

    DECLARE_META_INTERFACE(HWCPrivateService);

    virtual int switchDevice(const DeviceTable& tables) = 0;
    virtual int blank(int display, int enable) = 0;
    virtual int setOutputMode(int display, int type, int mode) = 0;

    virtual int setMargin(int display, int l, int r, int t, int b) = 0;
    virtual int setVideoRatio(int display, int ratio) = 0;
    virtual int set3DMode(int display, int mode) = 0;
    virtual int setDataspace(int display, int dataspace) = 0;
    virtual int registerCallback(sp<IBinder>& binder) = 0;

    virtual void setDebugTag(int32_t tag) = 0;
};

class BnHWCPrivateService : public BnInterface<IHWCPrivateService>
{
public:
    virtual status_t onTransact(uint32_t code,
                const Parcel& data, Parcel* reply, uint32_t flags = 0);
};

};
#endif

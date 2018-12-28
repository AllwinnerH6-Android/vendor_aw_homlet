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

#ifndef __ICALLBACK_H__
#define __ICALLBACK_H__

#include <sys/types.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>

namespace android {

class IDisplaydCallback : public IInterface
{
public:
    DECLARE_META_INTERFACE(DisplaydCallback);
    virtual int dataspaceChange(int m) = 0;
};

class BnDisplaydCallback : public BnInterface<IDisplaydCallback>
{
public:
    virtual status_t onTransact(uint32_t code,
                const Parcel& data, Parcel* reply, uint32_t flags = 0);
};

};

#endif


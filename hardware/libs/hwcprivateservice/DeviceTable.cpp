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

#include "DeviceTable.h"

bool DeviceTable::isFixedSize() const
{
    return false;
}

size_t DeviceTable::getFlattenedSize() const
{
    size_t size = sizeof(device_info_t) * mTables.size();
    return size;
}


android::status_t DeviceTable::flatten(void *buffer, size_t size) const
{
    if (size < getFlattenedSize())
        return android::NO_MEMORY;

    memcpy(buffer, mTables.array(), getFlattenedSize());
    return android::NO_ERROR;
}

android::status_t DeviceTable::unflatten(void const* buffer, size_t size)
{
    if (size < sizeof(device_info_t))
        return android::NO_MEMORY;

    size_t tabSize = size / sizeof(device_info_t);
    mTables.resize(tabSize);
    memcpy(mTables.editArray(), buffer, sizeof(device_info_t) * tabSize);
    return android::NO_ERROR;
}


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

#include "utils.h"
#include "debug.h"
#include "output_policy.h"
#include "sunxi_display2.h"

const char *OutputPolicy::PolicyType::string() const
{
    switch (mSetting) {
    case OUTPUT_POLICY_SINGLE: return "Single display";
    case OUTPUT_POLICY_DUAL:   return "dual display";
    default:                   return "unknown";
    }
}

OutputPolicy::OutputPolicy(int type)
  : mType(type),
    mCurrentMapping()
{
    mConnectStates.emplace(PRIMARY_DEVICE,  1);
    mConnectStates.emplace(EXTERNAL_DEVICE, 0);
}

void OutputPolicy::updateConnectState(int type, int connected)
{
    dd_debug("updateConnectState: type=%d connected=%d", type, connected);
    std::unique_lock<std::mutex> lock(mLock);

    for (auto& item : mConnectStates) {
        int type = item.first;
        mConnectStates[type] = getConnectState(type);
        dd_debug("updated: type=%d connected=%d",
                 type, mConnectStates[type]);
    }

    /*
     * On first trigger, wait enought time for primary display setup.
     * In case the switch uevent of primary display is later than the
     * external display.
     */
    if (type == -1 && connected == -1
            && mConnectStates[PRIMARY_DEVICE] != 1) {
        usleep(1000 * 100);
        mConnectStates[PRIMARY_DEVICE] = getConnectState(PRIMARY_DEVICE);
        dd_debug("updateConnectState on primary after 100 ms, connected = %d",
                 mConnectStates[PRIMARY_DEVICE]);
    }

    rebuildMapping();
}

void OutputPolicy::getOutputDeviceMapping(std::vector<OutputDeviceInfo>& out)
{
    std::unique_lock<std::mutex> lock(mLock);

    out.clear();
    for (auto& item : mCurrentMapping) {
        out.push_back(OutputDeviceInfo(item.second));
    }
}

const OutputDeviceInfo *OutputPolicy::getOutputDeviceInfoById(const int logicalId)
{
    std::unique_lock<std::mutex> lock(mLock);
    if (mCurrentMapping.count(logicalId))
        return &mCurrentMapping[logicalId];
    return nullptr;
}

#define PRIMARY_DEVICE_STATE_PATH   "/sys/class/switch/hdmi/state"
#define EXTERNAL_DEVICE_STATE_PATH  "/sys/class/switch/cvbs/state"

int OutputPolicy::getConnectState(int type)
{
    int connected = 0;
    switch (type) {
    case PRIMARY_DEVICE:
        connected = getConnectStateFromFile(PRIMARY_DEVICE_STATE_PATH);
        break;
    case EXTERNAL_DEVICE:
        connected = getConnectStateFromFile(EXTERNAL_DEVICE_STATE_PATH);
        break;
    default:
        connected = -1;
    }
    if (connected < 0) {
        dd_error("getConnectState failed, connected = -1");
        connected = 0;
    }
    return connected;
}

/* --------------------------------------------------------------------------------*/

SingleDisplayPolicy::SingleDisplayPolicy()
    : OutputPolicy(OUTPUT_POLICY_SINGLE) {}

int SingleDisplayPolicy::rebuildMapping()
{
    mCurrentMapping.clear();
    if (mConnectStates[PRIMARY_DEVICE] == 1) {
        mCurrentMapping.emplace(0, OutputDeviceInfo(0, PRIMARY_DEVICE,  1));
        mCurrentMapping.emplace(1, OutputDeviceInfo(1, EXTERNAL_DEVICE, 0));
    } else if (mConnectStates[EXTERNAL_DEVICE] == 1) {
        mCurrentMapping.emplace(0, OutputDeviceInfo(0, EXTERNAL_DEVICE, 1));
        mCurrentMapping.emplace(1, OutputDeviceInfo(1, PRIMARY_DEVICE,  0));
    } else {
        mCurrentMapping.emplace(0, OutputDeviceInfo(0, PRIMARY_DEVICE,  1));
        mCurrentMapping.emplace(1, OutputDeviceInfo(1, EXTERNAL_DEVICE, 0));
    }
    return 0;
}

/* --------------------------------------------------------------------------------*/

DualDisplayPolicy::DualDisplayPolicy()
    : OutputPolicy(OUTPUT_POLICY_DUAL) {}

int DualDisplayPolicy::rebuildMapping()
{
    mCurrentMapping.clear();
    if (mConnectStates[PRIMARY_DEVICE] == 1) {
        mCurrentMapping.emplace(0, OutputDeviceInfo(0, PRIMARY_DEVICE, 1));
        mCurrentMapping.emplace(1,
            OutputDeviceInfo(1, EXTERNAL_DEVICE, mConnectStates[EXTERNAL_DEVICE]));
    } else {
        if (mConnectStates[EXTERNAL_DEVICE] == 1) {
            mCurrentMapping.emplace(0, OutputDeviceInfo(0, EXTERNAL_DEVICE, 1));
            mCurrentMapping.emplace(1, OutputDeviceInfo(1, PRIMARY_DEVICE,  0));
        } else {
            mCurrentMapping.emplace(0, OutputDeviceInfo(0, PRIMARY_DEVICE,  1));
            mCurrentMapping.emplace(1, OutputDeviceInfo(1, EXTERNAL_DEVICE, 0));
        }
    }
    return 0;
}


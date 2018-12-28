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

#ifndef __OUTPUT_POLICY_H__
#define __OUTPUT_POLICY_H__

#include <mutex>
#include <vector>
#include <unordered_map>
#include "platform.h"

#define OUTPUT_POLICY_SINGLE    (1)
#define OUTPUT_POLICY_DUAL      (2)

struct OutputDeviceInfo {
    OutputDeviceInfo()
      : logicalId(-1), type(0), enabled(0)
    {}

    OutputDeviceInfo(int id, int t, int e)
      : logicalId(id), type(t), enabled(e)
    {}

    OutputDeviceInfo(const OutputDeviceInfo& r)
      : logicalId(r.logicalId),
        type(r.type),
        enabled(r.enabled)
    {}

    int logicalId;
    int type;
    int enabled;
};

class OutputPolicy {
public:
    OutputPolicy(int type);
    virtual ~OutputPolicy() {};

    void updateConnectState(int type, int connected);
    void getOutputDeviceMapping(std::vector<OutputDeviceInfo>& out);
    const OutputDeviceInfo *getOutputDeviceInfoById(const int logicalId);

    class PolicyType {
    public:
        int mSetting;
        const char *string() const;

        PolicyType(int type)
          : mSetting(type)
        {}
    };
    const char *name() {
        return mType.string();
    }

protected:
    virtual int rebuildMapping() = 0;
    virtual int getConnectState(int type);

    const PolicyType mType;

    /* mConnectStates: type(key) connected=1/0(value) */
    std::unordered_map<int, int> mConnectStates;
    /* mCurrentMapping: logicalId(key) device info(value) */
    std::unordered_map<int, OutputDeviceInfo> mCurrentMapping;
    std::mutex mLock;
};

class SingleDisplayPolicy : public OutputPolicy {
public:
	SingleDisplayPolicy();

private:
	virtual int rebuildMapping();
};

class DualDisplayPolicy : public OutputPolicy {
public:
	DualDisplayPolicy();

private:
	virtual int rebuildMapping();
};

OutputPolicy *outputPolicyInit();

#endif

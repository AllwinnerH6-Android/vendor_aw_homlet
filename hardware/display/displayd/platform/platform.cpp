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

#include <cutils/properties.h>

#include "debug.h"
#include "platform.h"
#include "device/device_hdmi.h"
#include "device/device_cvbs.h"
#include "output_policy.h"

void initDisplayDevice(std::vector<DeviceControler*>& out)
{
    out.push_back(new HdmiDevice());
    out.push_back(new CvbsDevice());
}

OutputPolicy *outputPolicyInit()
{
    OutputPolicy *p = nullptr;
    int type = property_get_int32(
                    DISPLAY_POLICY_PROPERTY,
                    DISPLAY_POLICY_TYPE_DEFAULT);

    switch (type) {
    case DISPLAY_POLICY_TYPE_SINGLE:
        dd_info("Select output policy: single display");
        p = new SingleDisplayPolicy();
        return p;
    case DISPLAY_POLICY_TYPE_DUAL:
        dd_info("Select output policy: dual display");
        p = new DualDisplayPolicy();
        return p;
    default:
        dd_error("unknow display policy config: %d", type);
        return nullptr;
    }
}

#define DEVICE_HDMI_STATE_PATH  "/sys/class/switch/hdmi/state"
#define DEVICE_CVBS_STATE_PATH  "/sys/class/switch/cvbs/state"

int platformGetDeviceConnectState(int type)
{
    int connected = 0;
    switch (type) {
    case DISP_OUTPUT_TYPE_HDMI:
        connected = getConnectStateFromFile(DEVICE_HDMI_STATE_PATH);
        break;
    case DISP_OUTPUT_TYPE_TV:
        connected = getConnectStateFromFile(DEVICE_CVBS_STATE_PATH);
        break;
    default:
        connected = -1;
    }
    if (connected < 0) {
        dd_error("platformGetDeviceConnectState failed");
        connected = 0;
    }
    return connected;
}

class DefaultEnhance : public HardwareCtrl::EnhanceBase {
public:
    int getEnhanceComponent(int option) {
        return getComponent(option);
    }
    int setEnhanceComponent(int option, int t) {
        return setComponent(option, t);
    }
    int setup() {
        setComponent(kEnhanceBright, 5);
        setComponent(kEnhanceContrast, 0);
        setComponent(kEnhanceSaturation, 4);
        setComponent(kEnhanceDenoise, 5);
        setComponent(kEnhanceDetail, 2);
        setComponent(kEnhanceMode, 1);
        return 0;
    }

private:
    const char *_prefix = "/sys/class/disp/disp/attr";
    const char *_attr_list[7] = {
        "enhance_mode",       /* kEnhanceMode       = 0 */
        "enhance_bright",     /* kEnhanceBright     = 1 */
        "enhance_contrast",   /* kEnhanceContrast   = 2 */
        "enhance_denoise",    /* kEnhanceDenoise    = 3 */
        "enhance_detail",     /* kEnhanceDetail     = 4 */
        "enhance_edge",       /* kEnhanceEdge       = 5 */
        "enhance_saturation", /* kEnhanceSaturation = 6 */
    };

    int getComponent(int option) {
        if (option >= (int)NELEM(_attr_list) || option < 0) {
            dd_error("DefaultEnhance: unknow component (%d)", option);
            return -1;
        }
        char path[128] = {0};
        sprintf(path, "%s/%s", _prefix, _attr_list[option]);
        return readIntFromFile(path);
    }

    int setComponent(int option, int t) {
        if (option >= (int)NELEM(_attr_list) || option < 0) {
            dd_error("DefaultEnhance: unknow component (%d)", option);
            return -1;
        }
        char path[128] = {0};
        sprintf(path, "%s/%s", _prefix, _attr_list[option]);
        if (t < 0)  t = 0;
        if (t > 10) t = 10;
        return writeIntToFile(path, t);
    }
};

HardwareCtrl::EnhanceBase *platformGetEnhanceHandle(int id)
{
    if (id == 0)
        return new DefaultEnhance();
    return nullptr;
}


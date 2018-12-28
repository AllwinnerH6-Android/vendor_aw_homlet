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

#ifndef __EDID_STRATEGY_H__
#define __EDID_STRATEGY_H__

#include "edid.h"
#include "platform.h"

class EdidStrategy {
public:
    EdidStrategy();

    int update();
    int checkAndCorrectDeviceConfig(struct disp_device_config *config);
    bool supportedHDR();
    bool supported3DPresent();
    bool supportedPixelFormat(int mode, int format, int depth);

    /*
     * Setup device config for target dataspace mode.
     * RETURN: return true on success.
     * If dirty != 0, update config to hardware.
     */
    bool setupConfig(int target, struct disp_device_config *config, int *dirty);
    void dump(android::String8& str) {
        mParser.dump(str);
    }

    enum eKdataspace {
        SDR = 0,
        HDR = 1,
        WCG = 2,
    };
private:
    static int getColorspaceByResolution(int mode);
    bool isSupportY420Sampling(int mode);
    bool isOnlySupportY420Sampling(int mode);
    int checkSamplingBits(int format, int bits);

    EdidParser mParser;
};

#endif

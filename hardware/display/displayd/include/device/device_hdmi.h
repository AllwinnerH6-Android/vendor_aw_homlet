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

#include "device_controler.h"
#include "edid_strategy.h"

#define DATASPACE_SDR (0x1001)
#define DATASPACE_HDR (0x1002)

class HdmiDevice : public DeviceControler {
public:
    HdmiDevice();
   ~HdmiDevice();

    int performOptimalConfig(int enable);
    int performDefaultConfig(int enable);

    int getSupportModeList(std::vector<int>& out);
    int getSupportPixelFormat(std::vector<int>& out);
    int getSupportDataspace(std::vector<int>& out);
    bool isSupportMode(int mode);
    bool isSupport3D();

    int setDisplayModeImpl(int mode);
    int setDataspaceMode(int mode);
    int setPixelFormat(int format);
    int onDataspaceChange(int dataspace);

private:
    void userSetting2Config(disp_device_config *config);
    int performOptimalMode();
    static void dumpDeviceConfig(const char *prefix,
                                 struct disp_device_config *config);
    void dumpHook(android::String8& str);

    /*
     * User specific Perfect config.
     * format: RGB/YUV444/YUV422/YUV420
     * depth : 8bit/10bit/12bit
     */
    int mPerfectFormat;
    int mPerfectDepth;
    EdidStrategy mEdidStrategy;

    /*
     * Target dataspace request from hwcomposer
     */
    int mRequestDataspace;
    int mCurrent3dMode;
};
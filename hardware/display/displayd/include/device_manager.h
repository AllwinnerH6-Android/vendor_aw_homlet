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

#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include "DisplayManagerIntf.h"
#include "hotplug_monitor.h"
#include "output_policy.h"
#include "device/device_controler.h"
#include "mq_thread.h"

class DeviceManager: public DisplayManagerIntf {
public:
    DeviceManager();
  	virtual ~DeviceManager() {};

    void initialize();
    void hotplug(int type, int connected);
    void waitHwcAdapterSetup();

private:
    /*
     * Interface for displayd client.
     * Protect by std::mutex mLock
     */
    int getType(int display);
    int getMode(int display);
    int setMode(int display, int mode);
    int isSupportedMode(int display, int mode);
    int getSupportedModes(int display, std::vector<int>& outlist);

    int setAspectRatio(int display, int ratio);
    int getAspectRatio(int display);
    int setMargin(int display, int l, int r, int t, int b);
    int getMargin(int display, std::vector<int>& out);

    int isSupported3D(int display);
    int get3DLayerMode(int display);
    int set3DLayerMode(int display, int mode);

    int getSupportedPixelFormats(int display, std::vector<int>& formats);
    int getPixelFormat(int display);
    int setPixelFormat(int display, int format);

    int getCurrentDataspace(int display);
    int getDataspaceMode(int display);
    int setDataspaceMode(int display, int mode);

    int getEnhanceComponent(int display, int option);
    int setEnhanceComponent(int display, int option, int value);

    /*
     * Dump debug info for DisplayDebug tool
     */
    int dump(std::string& out);

    /*
     * Callback from HWC when dataspace change
     */
    int onDataspaceChange(int dataspace);
    void handleDataspaceChange(int dataspace);

private:
    inline DeviceControler *getControler(int type);
    inline DeviceControler *getControlerById(int id);
    void setupDefaultOutput();

    class HotplugCallback;
    const HotplugCallback *mHotplugCallback;
    HotplugMonitor *mHotplugMonitor;

    class HWCCallback;
    class DeathObserver;
    android::sp<DeathObserver> mAdapterObserver;
    android::sp<HWCCallback> mHwcCallback;
    android::sp<MessageQueueThread> mMessageThread;

    OutputPolicy *mPolicy;
    std::vector<DeviceControler*> mDevices;
    std::unordered_map<int, DeviceControler*> mDisplayDevices;

    std::mutex mLock;
};

#endif

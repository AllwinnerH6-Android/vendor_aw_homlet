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

#ifndef __DISPLAY_MANAGER_INTF_H__
#define __DISPLAY_MANAGER_INTF_H__

class DisplayManagerIntf {
public:
    virtual ~DisplayManagerIntf() {};

    /*
     * Interface for displayd client.
     * Protect by std::mutex mLock
     */
    virtual int getType(int display) = 0;
    virtual int getMode(int display) = 0;
    virtual int setMode(int display, int mode) = 0;
    virtual int isSupportedMode(int display, int mode) = 0;
    virtual int getSupportedModes(int display, std::vector<int>& outlist) = 0;

    virtual int setAspectRatio(int display, int ratio) = 0;
    virtual int getAspectRatio(int display) = 0;
    virtual int setMargin(int display, int l, int r, int t, int b) = 0;
    virtual int getMargin(int display, std::vector<int>& out) = 0;

    virtual int isSupported3D(int display) = 0;
    virtual int get3DLayerMode(int display) = 0;
    virtual int set3DLayerMode(int display, int mode) = 0;

    virtual int getSupportedPixelFormats(int display, std::vector<int>& formats) = 0;
    virtual int getPixelFormat(int display) = 0;
    virtual int setPixelFormat(int display, int format) = 0;

    virtual int getCurrentDataspace(int display) = 0;
    virtual int getDataspaceMode(int display) = 0;
    virtual int setDataspaceMode(int display, int mode) = 0;

    virtual int getEnhanceComponent(int display, int option) = 0;
    virtual int setEnhanceComponent(int display, int option, int value) = 0;

    virtual int dump(std::string& out) = 0;

	static DisplayManagerIntf *createInstance();
};

#endif

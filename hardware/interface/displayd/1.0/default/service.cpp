/*
 * Copyright 2016 The Android Open Source Project
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

#define LOG_TAG "displayd"

#include <sched.h>
#include <binder/ProcessState.h>
#include <hidl/HidlTransportSupport.h>
#include "Displayd.h"
#include "DisplayManagerIntf.h"

using softwinner::homlet::displayd::V1_0::IDisplayd;
using softwinner::homlet::displayd::V1_0::implementation::Displayd;

int main() {
    // the conventional HAL might start binder services
    android::ProcessState::initWithDriver("/dev/vndbinder");
    android::ProcessState::self()->setThreadPoolMaxThreadCount(4);
    android::ProcessState::self()->startThreadPool();

    DisplayManagerIntf *intf = DisplayManagerIntf::createInstance();

    // Setup hwbinder service
    android::sp<IDisplayd> service = new Displayd(intf);
    android::status_t status = service->registerAsService();
    LOG_ALWAYS_FATAL_IF(
            status != android::OK,
            "Error while registering displayd service: %d", status);

    android::hardware::joinRpcThreadpool();
    return 0;
}

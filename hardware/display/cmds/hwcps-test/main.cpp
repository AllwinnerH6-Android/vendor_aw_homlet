

#include <stdlib.h>
#include <inttypes.h>
#include <utils/String16.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <system/thread_defs.h>

#include "hwcps.h"

Hwcps* Hwcps::mInstance = nullptr;
Hwcps* Hwcps::instantiate()
{
    if (mInstance == nullptr)
        mInstance = new Hwcps();
    return mInstance;
}

int Hwcps::blank(int display, int enable) {
    ALOGD("blank: display[%d], enable[%d]",
          display, enable);
    return 0;
}

int Hwcps::switchDevice(const DeviceTable& tables) {
    ALOGD("switchDevice: phy display count %d",
          tables.mTables.size());
    return 0;
}

int Hwcps::setOutputMode(int display, int type, int mode) {
    ALOGD("setOutputMode: display[%d] type=%d mode=%d",
          display, type, mode);
    return 0;
}

int Hwcps::setMargin(int display, int l, int r, int t, int b) {
    ALOGD("setMargin: display[%d] l=%d r=%d t=%d b=%d",
          display, l, r, t, b);
    return 0;
}

int Hwcps::setVideoRatio(int display, int ratio) {
    ALOGD("setVideoRatio: display[%d], ratio=%d",
          display, ratio);
    return 0;
}

int Hwcps::set3DMode(int display, int mode) {
    ALOGD("set3DMode: display[%d], mode=%d",
          display, mode);
    return 0;
}

int Hwcps::setDataspace(int display, int dataspace) {
    ALOGD("setDataspace: display[%d], dataspace=%d",
          display, dataspace);
    return 0;
}

int Hwcps::registerCallback(android::sp<android::IBinder>& binder) {
    return 0;
}

void Hwcps::setDebugTag(int32_t tag) {
    ALOGD("setDebugTag: tag=%d", tag);
}

android::status_t Hwcps::dump(int fd, const android::Vector<android::String16>& args) {

    android::String8 result;
    result.appendFormat("HWCPrivateService:\n");
    result.appendFormat("  DeviceSlot: none\n");

    write(fd, result.string(), result.size());
    return android::NO_ERROR;
}

void Hwcps::init() {
    /* Register Hwcps */
    android::defaultServiceManager()->addService(
            android::String16("HWCPrivateService"), this);
    ALOGD("HWCPrivateService: init");
}

int main() {
    // start binder services
    android::ProcessState::initWithDriver("/dev/vndbinder");
    android::ProcessState::self()->setThreadPoolMaxThreadCount(4);
    android::ProcessState::self()->startThreadPool();

    // publish service
    Hwcps *privateService = Hwcps::instantiate();
    privateService->init();

    android::IPCThreadState::self()->joinThreadPool();
    return 0;
}

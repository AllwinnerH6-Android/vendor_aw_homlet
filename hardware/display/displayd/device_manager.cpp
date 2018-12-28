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

#include "debug.h"
#include "IDisplaydCallback.h"
#include "HWCProxy.h"
#include "device_manager.h"
#include "platform.h"

class DeviceManager::HotplugCallback : public HotplugMonitor::Callback {
public:
    HotplugCallback(DeviceManager &manager) : mManager(manager) {
        hotplug = hotplugHook;
    }

    static void hotplugHook(const HotplugMonitor::Callback *cb,
                            int type, int connected) {
        auto callback = static_cast<const HotplugCallback *>(cb);
        callback->mManager.hotplug(type, connected);
    }

private:
    DeviceManager& mManager;
};

class DeviceManager::DeathObserver:
        public android::HWCProxy::Callback, public android::Thread {
    DeviceManager &mManager;
    mutable android::Mutex mLock;
    mutable android::Condition mCondition;

public:
    DeathObserver(DeviceManager &manager)
        : mManager(manager) {
        deathNotify = deathHook;
    }
    static void deathHook(const android::HWCProxy::Callback *cb) {
        auto instance = static_cast<const DeathObserver *>(cb);
        instance->death();
    }

protected:
    void death() const {
        android::Mutex::Autolock _l(mLock);
        mCondition.broadcast();
        dd_info("HwcAdapterService has died");
    }

    virtual bool threadLoop() {
        android::Mutex::Autolock _l(mLock);
        mCondition.wait(mLock);

        mManager.waitHwcAdapterSetup();
        return true;
    }
};

class DeviceManager::HWCCallback: public android::BnDisplaydCallback {
public:
    HWCCallback(DeviceManager& dm) : mDeviceManager(dm) { }
   ~HWCCallback() { };

private:
    virtual int dataspaceChange(int m) {
        std::unique_lock<std::mutex> lock(mLock);
        return mDeviceManager.onDataspaceChange(m);
    }

    std::mutex mLock;
    DeviceManager& mDeviceManager;
};

DeviceManager::DeviceManager()
  : mHotplugCallback(nullptr)
{
    initialize();
    mHotplugCallback = new HotplugCallback(*this);
    mHotplugMonitor =
        new HotplugMonitor(static_cast<const HotplugMonitor::Callback *>(mHotplugCallback),
                           HotplugMonitor::createUeventSocket());

    mAdapterObserver = new DeathObserver(*this);
    mAdapterObserver->run("DeathObserver-thread");

    android::HWCProxy::registerDeathNotify(mAdapterObserver.get());

    // MessageQueue thread initialize
    mMessageThread = new MessageQueueThread();
    mMessageThread->run("MQ-Thread");
}

void DeviceManager::initialize()
{
    mPolicy = outputPolicyInit();
    initDisplayDevice(mDevices);

#if 0
    /* FIXME: just for debug */
    DeviceControler *controler = getControler(PRIMARY_DEVICE);
    controler->setLogicalId(0);
    mDisplayDevices.clear();
    mDisplayDevices.emplace(0, controler);
#else
    /* Trigger a hotplug to rebuild device mapping */
    dd_info("Initizlize display device mapping");
    setupDefaultOutput();

    /* Register callback for dataspace change notify */
    auto hwcservice = android::HWCProxy::getHwcService();
    mHwcCallback = new HWCCallback(*this);
    android::sp<android::IBinder> binder(mHwcCallback.get());
    hwcservice->registerCallback(binder);
#endif
}

void DeviceManager::waitHwcAdapterSetup()
{
    dd_info("HwcAdapterService death, try to reconnect...");

    {
        /*
         * getHwcService() will block here,
         * untill the remote service is setup.
         */
        std::unique_lock<std::mutex> lock(mLock);
        auto hwcservice = android::HWCProxy::getHwcService();
        mHwcCallback = new HWCCallback(*this);
        android::sp<android::IBinder> binder(mHwcCallback.get());
        hwcservice->registerCallback(binder);
    }
    setupDefaultOutput();
}

static void dumpDeviceMapping(DeviceTable& map)
{
    char outbuf[256] = {0};
    char *p = outbuf;
    p += sprintf(p, "Current device mapping:\n");
    for (auto& t : map.mTables) {
        p += sprintf(p, "  + display [%d]: enable=%d type=%d mode=%d\n",
                     t.logicalId, t.enabled, t.type, t.mode);
    }
    dd_info("%s", outbuf);
}

void DeviceManager::setupDefaultOutput()
{
    std::unique_lock<std::mutex> lock(mLock);

    /* Force a rebuild to get current output mapping. */
    std::vector<OutputDeviceInfo> mapping;
    mPolicy->updateConnectState(-1, -1);
    mPolicy->getOutputDeviceMapping(mapping);

    DeviceTable routing;
    mDisplayDevices.clear();
    for (size_t i = 0; i < mapping.size(); i++) {
        int type = mapping[i].type;
        int id   = mapping[i].logicalId;

        DeviceControler *controler = getControler(type);
        if (!controler) {
            dd_error("Skip unknow device, type=%d", type);
            continue;
        }
        controler->setLogicalId(id);
        controler->performDefaultConfig(mapping[i].enabled);
        mDisplayDevices.emplace(id, controler);

        routing.mTables.push_back(
                DeviceTable::device_info(
                    id, controler->type(), controler->getDisplayMode(),
                    mapping[i].enabled));
    }

#if 1
    auto hwcservice = android::HWCProxy::getHwcService();
    if (hwcservice->switchDevice(routing) != 0) {
        dd_error("SwitchDevice notify return error");
    }

    /* Unblank and present */
    for (const auto& device : mDisplayDevices) {
        device.second->present();
    }
#endif

    dumpDeviceMapping(routing);
}

inline DeviceControler *DeviceManager::getControler(int type)
{
    for (auto iter = mDevices.begin();
              iter != mDevices.end(); ++iter) {
        if ((*iter)->type() == type)
            return *iter;
    }
    dd_error("Cant't fine device controler (type=%d)", type);
    return nullptr;
}

inline DeviceControler *DeviceManager::getControlerById(int id)
{
    if (mDisplayDevices.count(id) != 1) {
        dd_error("Cant't fine device controler (id=%d)", id);
        return nullptr;
    }
    return mDisplayDevices[id];
}

void DeviceManager::hotplug(int type, int connected)
{
    std::unique_lock<std::mutex> lock(mLock);

    std::vector<OutputDeviceInfo> mapping;

    /* 1. Trigger outputPolicy to rebuild device mapping */
    mPolicy->updateConnectState(type, connected);
    mPolicy->getOutputDeviceMapping(mapping);

    /* 2. Update logical device map */
    DeviceTable routing;
    mDisplayDevices.clear();
    for (size_t i = 0; i < mapping.size(); i++) {
        int type = mapping[i].type;
        int id   = mapping[i].logicalId;

        DeviceControler *controler = getControler(type);
        if (!controler) {
            dd_error("Skip unknow device, type=%d", type);
            continue;
        }
        controler->setLogicalId(id);
        controler->performOptimalConfig(mapping[i].enabled);
        mDisplayDevices.emplace(id, controler);

        routing.mTables.push_back(
                DeviceTable::device_info(
                    id, controler->type(), controler->getDisplayMode(),
                    mapping[i].enabled));
    }

    /* 3. Notify hwc adapter service */
    auto hwcservice = android::HWCProxy::getHwcService();
    if (hwcservice->switchDevice(routing) != 0) {
        dd_error("SwitchDevice notify return error");
    }

    /* 4. Unblank and present/setup margin */
    for (const auto& device : mDisplayDevices) {
        device.second->present();
    }

    /* 5. debug output */
    dumpDeviceMapping(routing);
}

/*
 * Interface for displayd client.
 * Protect by std::mutex mLock
 */

int DeviceManager::getType(int display)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->type();
    return -1;
}

int DeviceManager::getMode(int display)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->getDisplayMode();
    return -1;
}

int DeviceManager::setMode(int display, int mode)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (!controler || !controler->isSupportMode(mode)) {
        dd_error("device(type=%d) not support mode %d", controler->type(), mode);
        return -1;
    }
    return controler->setDisplayMode(mode);
}

int DeviceManager::isSupportedMode(int display, int mode)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->isSupportMode(mode) ? 1 : 0;
    return 0;
}

int DeviceManager::getSupportedModes(int display, std::vector<int>& outlist)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->getSupportModeList(outlist);
    return -1;
}

int DeviceManager::setAspectRatio(int display, int ratio)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->setAspectRatio(ratio);
    return -1;
}

int DeviceManager::getAspectRatio(int display)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->getAspectRatio();
    return -1;
}

int DeviceManager::setMargin(int display, int l, int r, int t, int b)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->setDisplayMargin(l, r, t, b);
    return -1;
}

int DeviceManager::getMargin(int display, std::vector<int>& out)
{
    int ret = -1;
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler) {
        int margin[4] = {0};
        ret = controler->getDisplayMargin(margin);
        out.push_back(margin[0]);
        out.push_back(margin[1]);
        out.push_back(margin[2]);
        out.push_back(margin[3]);
    }
    return ret;
}

int DeviceManager::isSupported3D(int display)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->isSupport3D();
    return 0;
}

int DeviceManager::get3DLayerMode(int display)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->get3DLayerMode();
    return 0;
}

int DeviceManager::set3DLayerMode(int display, int mode)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->set3DLayerMode(mode);
    return 0;
}

int DeviceManager::getSupportedPixelFormats(int display, std::vector<int>& formats)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->getSupportPixelFormat(formats);
    return 0;
}

int DeviceManager::getPixelFormat(int display)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->getPixelFormat();
    return 0;
}

int DeviceManager::setPixelFormat(int display, int format)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->setPixelFormat(format);
    return 0;
}

int DeviceManager::getCurrentDataspace(int display)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->getCurrentDataspace();
    return 0;
}

int DeviceManager::getDataspaceMode(int display)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->getDataspaceMode();
    return 0;
}

int DeviceManager::setDataspaceMode(int display, int mode)
{
    std::unique_lock<std::mutex> lock(mLock);
    DeviceControler *controler = getControlerById(display);
    if (controler)
        return controler->setDataspaceMode(mode);
    return 0;
}

int DeviceManager::getEnhanceComponent(int display, int option)
{
    DeviceControler *controler = getControlerById(display);
    if (controler && controler->getEnhanceHandle()) {
        return controler->getEnhanceHandle()->getEnhanceComponent(option);
    }
    return 0;
}

int DeviceManager::setEnhanceComponent(int display, int option, int value)
{
    DeviceControler *controler = getControlerById(display);
    if (controler && controler->getEnhanceHandle()) {
        return controler->getEnhanceHandle()->setEnhanceComponent(option, value);
    }
    return 0;
}

int DeviceManager::onDataspaceChange(int dataspace)
{
    dd_info("onDataspaceChange: request dataspace 0x%08x", dataspace);

    class DataspaceChangeMsg: public MessageBase {
        DeviceManager *manager;
        int dataspace;
    public:
        DataspaceChangeMsg(DeviceManager *m, int d)
            : manager(m), dataspace(d) {
        }

        virtual bool handler() {
            manager->handleDataspaceChange(dataspace);
            return true;
        }
    };

    /*
     * device->onDataspaceChange() will callback into hwc,
     * So, use async handle here to avoid death lock.
     */
    android::sp<MessageBase> msg = new DataspaceChangeMsg(this, dataspace);
    mMessageThread->postMessage(msg);
    return 0;
}

void DeviceManager::handleDataspaceChange(int dataspace)
{
    dd_info("handleDataspaceChange: dataspace 0x%08x", dataspace);
    if (mDisplayDevices.count(0) == 1) {
        DeviceControler *device = mDisplayDevices[0];
        device->onDataspaceChange(dataspace);
    }
}

int DeviceManager::dump(std::string& out)
{
    android::String8 buf;
    std::unique_lock<std::mutex> lock(mLock);

    buf.appendFormat("Hotplug policy: %s\n", mPolicy->name());
    buf.appendFormat("Devices (%d entries)\n", mDisplayDevices.size());
    for (const auto& device : mDisplayDevices) {
        buf.appendFormat(" + Device [%d] ==>\n", device.first);
        device.second->dump(buf);
    }
    out.append(buf.string());
    return 0;
}

/* static */ DisplayManagerIntf* DisplayManagerIntf::createInstance() {
    return new DeviceManager();
}


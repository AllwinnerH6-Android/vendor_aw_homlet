
#include <utils/Log.h>
#include "Displayd.h"

namespace softwinner {
namespace homlet {
namespace displayd {
namespace V1_0 {
namespace implementation {

#define DEBUG
#ifdef  DEBUG
#define debug(x, ...) ALOGD("%s: " x, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

Displayd::Displayd(DisplayManagerIntf *intf)
    : mInterface(intf) { }

Displayd::~Displayd() {

}

// Methods from IDisplayd follow.
Return<IfaceType> Displayd::getType(int32_t display) {
    int type = mInterface->getType(display);
    if (type < 0)
        return IfaceType::NONE;

    /*
    debug("display[%d] Current type: %s",
            display, toString(IfaceType(type)).c_str());
    */
    return IfaceType(type);
}

Return<DispFormat> Displayd::getMode(int32_t display) {
    int mode = mInterface->getMode(display);
    debug("display[%d] Current formt: %s",
            display, toString(DispFormat(mode)).c_str());
    return DispFormat(mode);
}

Return<Error> Displayd::setMode(int32_t display, DispFormat fmt) {
    debug("display[%d] fmt[%s]", display, toString(fmt).c_str());
    if (!mInterface->setMode(display, static_cast<int32_t>(fmt)))
        return Error::NONE;
    return Error::UNSUPPORTED;
}

Return<bool> Displayd::isSupportedMode(int32_t display, DispFormat fmt) {
    debug("display[%d] fmt[%s]", display, toString(fmt).c_str());
    if (mInterface->isSupportedMode(display, static_cast<int>(fmt)))
        return true;
    return false;
}

Return<void> Displayd::getSupportedModes(int32_t display, getSupportedModes_cb _hidl_cb) {
    std::vector<int32_t> support;
    std::vector<DispFormat> fmts;
    int ret = mInterface->getSupportedModes(display, support);

    debug("display[%d] ret=%d, supported mode count(%zd):",
            display, ret, support.size());

    if (!ret && support.size()) {
        for (size_t i = 0; i < support.size(); i++) {
            fmts.push_back(DispFormat(support[i]));
            debug("\t- %s", toString(DispFormat(support[i])).c_str());
        }
    }
    hidl_vec<DispFormat> out;
    out.setToExternal(fmts.data(), fmts.size());
    _hidl_cb(out);
    return Void();
}

Return<bool> Displayd::isSupported3D(int32_t display) {
    debug("display[%d]", display);
    if (mInterface->isSupported3D(display))
        return true;
    return false;
}

Return<LayerMode> Displayd::get3DLayerMode(int32_t display) {
    int mode = mInterface->get3DLayerMode(display);
    debug("display[%d] Current 3D LayerMode: %s",
            display, toString(LayerMode(mode)).c_str());
    return LayerMode(mode);
}

Return<Error> Displayd::set3DLayerMode(int32_t display, LayerMode mode) {
    debug("display[%d] mode[%s]", display, toString(mode).c_str());
    if (!mInterface->set3DLayerMode(display, static_cast<int32_t>(mode)))
        return Error::NONE;
    return Error::UNSUPPORTED;
}

Return<Error> Displayd::setAspectRatio(int32_t display, AspectRatio ratio) {
    debug("display[%d] AspectRatio[%s]", display, toString(ratio).c_str());
    if (!mInterface->setAspectRatio(display, static_cast<int32_t>(ratio)))
        return Error::NONE;
    return Error::UNSUPPORTED;
}

Return<AspectRatio> Displayd::getAspectRatio(int32_t display) {
    int ratio = mInterface->getAspectRatio(display);
    debug("display[%d] Current AspectRatio: %s",
            display, toString(AspectRatio(ratio)).c_str());
    return AspectRatio(ratio);
}

Return<Error> Displayd::setMargin(int32_t display, const ScreenMargin& margin) {
    debug("display[%d] Margin[%s]", display, toString(margin).c_str());
    if (!mInterface->setMargin(display,
            margin.left, margin.right, margin.top, margin.bottom)) {
        return Error::NONE;
    }
    return Error::UNSUPPORTED;
}

Return<void> Displayd::getMargin(int32_t display, getMargin_cb _hidl_cb) {
    std::vector<int> tmp;
    int ret = mInterface->getMargin(display, tmp);

    ScreenMargin margin;
    if (!ret && tmp.size() == 4) {
        margin.left   = tmp[0];
        margin.right  = tmp[1];
        margin.top    = tmp[2];
        margin.bottom = tmp[3];
    } else {
        margin.left   = 100;
        margin.right  = 100;
        margin.top    = 100;
        margin.bottom = 100;
    };
    _hidl_cb(margin);

    debug("display[%d] Margin[%s]", display, toString(margin).c_str());
    return Void();
}

Return<void> Displayd::getSupportedPixelFormats(int32_t display, getSupportedPixelFormats_cb _hidl_cb) {
    std::vector<int32_t> support;
    std::vector<PixelFormat> fmts;
    int ret = mInterface->getSupportedPixelFormats(display, support);

    debug("display[%d] ret=%d, supported pixel format count(%zd):",
            display, ret, support.size());

    if (!ret && support.size()) {
        for (size_t i = 0; i < support.size(); i++) {
            fmts.push_back(PixelFormat(support[i]));
            debug("\t- %s", toString(PixelFormat(support[i])).c_str());
        }
    }
    hidl_vec<PixelFormat> out;
    out.setToExternal(fmts.data(), fmts.size());
    _hidl_cb(out);
    return Void();
}

Return<PixelFormat> Displayd::getPixelFormat(int32_t display) {
    int fmt = mInterface->getPixelFormat(display);
    debug("display[%d] current PixelFormat: %s",
            display, toString(PixelFormat(fmt)).c_str());
    return PixelFormat(fmt);
}

Return<Error> Displayd::setPixelFormat(int32_t display, PixelFormat fmt) {
    debug("display[%d] PixelFormat[%s]", display, toString(fmt).c_str());
    if (!mInterface->setPixelFormat(display, static_cast<int32_t>(fmt)))
        return Error::NONE;
    return Error::UNSUPPORTED;
}

Return<Dataspace> Displayd::getCurrentDataspace(int32_t display) {
    int dataspace = mInterface->getCurrentDataspace(display);
    debug("display[%d] current Dataspace: %s",
            display, toString(Dataspace(dataspace)).c_str());
    return Dataspace(dataspace);
}

Return<Dataspace> Displayd::getDataspaceMode(int32_t display) {
    int mode = mInterface->getDataspaceMode(display);
    debug("display[%d] current Dataspace Mode: %s",
            display, toString(Dataspace(mode)).c_str());
    return Dataspace(mode);
}

Return<Error> Displayd::setDataspaceMode(int32_t display, Dataspace mode) {
    debug("display[%d] Dataspace[%s]", display, toString(mode).c_str());
    if (!mInterface->setDataspaceMode(display, static_cast<int32_t>(mode)))
        return Error::NONE;
    return Error::UNSUPPORTED;
}

Return<int32_t> Displayd::getEnhanceComponent(int32_t display, EnhanceItem item) {
    debug("display[%d] item[%s]", display, toString(item).c_str());
    return mInterface->getEnhanceComponent(display, static_cast<int32_t>(item));
}

Return<Error> Displayd::setEnhanceComponent(int32_t display, EnhanceItem item, int32_t value) {
    debug("display[%d] item[%s] value[%d]", display, toString(item).c_str(), value);
    if (!mInterface->setEnhanceComponent(display, static_cast<int32_t>(item), value))
        return Error::NONE;
    return Error::UNSUPPORTED;
}

Return<void> Displayd::dumpDebugInfo(dumpDebugInfo_cb hidl_cb) {
    std::string buf;
    mInterface->dump(buf);

    hidl_string buf_reply(buf.data(), buf.size());
    hidl_cb(buf_reply);

    return Void();
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

//IDisplayd* HIDL_FETCH_IDisplayd(const char* /* name */) {
//    return new Displayd();
//}

}  // implementation
}  // namespace V1_0
}  // namespace displayd
}  // namespace homlet
}  // namespace softwinner

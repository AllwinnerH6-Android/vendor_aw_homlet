#ifndef SOFTWINNER_HOMLET_DISPLAYD_V1_0_DISPLAYD_H
#define SOFTWINNER_HOMLET_DISPLAYD_V1_0_DISPLAYD_H

#include <softwinner/homlet/displayd/1.0/IDisplayd.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "DisplayManagerIntf.h"

namespace softwinner {
namespace homlet {
namespace displayd {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

class Displayd : public IDisplayd {
public:
    Displayd(DisplayManagerIntf *intf);

private:
    // Methods from IDisplayd follow.
    Return<IfaceType> getType(int32_t display) override;
    Return<DispFormat> getMode(int32_t display) override;
    Return<Error> setMode(int32_t display, DispFormat fmt) override;
    Return<bool> isSupportedMode(int32_t display, DispFormat fmt) override;
    Return<void> getSupportedModes(int32_t display, getSupportedModes_cb _hidl_cb) override;
    Return<bool> isSupported3D(int32_t display) override;
    Return<LayerMode> get3DLayerMode(int32_t display) override;
    Return<Error> set3DLayerMode(int32_t display, LayerMode mode) override;
    Return<Error> setAspectRatio(int32_t display, AspectRatio ratio) override;
    Return<AspectRatio> getAspectRatio(int32_t display) override;
    Return<Error> setMargin(int32_t display, const ScreenMargin& margin) override;
    Return<void> getMargin(int32_t display, getMargin_cb _hidl_cb) override;
    Return<void> getSupportedPixelFormats(int32_t display, getSupportedPixelFormats_cb _hidl_cb) override;
    Return<PixelFormat> getPixelFormat(int32_t display) override;
    Return<Error> setPixelFormat(int32_t display, PixelFormat fmt) override;
    Return<Dataspace> getCurrentDataspace(int32_t display) override;
    Return<Dataspace> getDataspaceMode(int32_t display) override;
    Return<Error> setDataspaceMode(int32_t display, Dataspace mode) override;
    Return<int32_t> getEnhanceComponent(int32_t display, EnhanceItem item) override;
    Return<Error> setEnhanceComponent(int32_t display, EnhanceItem item, int32_t value) override;
    Return<void> dumpDebugInfo(dumpDebugInfo_cb hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

    virtual ~Displayd();

private:
   	DisplayManagerIntf * const mInterface;
};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" IDisplayd* HIDL_FETCH_IDisplayd(const char* name);

}  // implementation
}  // namespace V1_0
}  // namespace displayd
}  // namespace homlet
}  // namespace softwinner

#endif  // SOFTWINNER_HOMLET_DISPLAYD_V1_0_DISPLAYD_H

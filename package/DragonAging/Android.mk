LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_STATIC_JAVA_LIBRARIES := android-support-v13
LOCAL_STATIC_JAVA_LIBRARIES := libsysapi
LOCAL_SRC_FILES := $(call all-java-files-under, src) \

LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_PACKAGE_NAME := DragonAging

# We mark this out until Mtp and MediaMetadataRetriever is unhidden.
#LOCAL_SDK_VERSION := current
LOCAL_CERTIFICATE := platform
LOCAL_STATIC_JAVA_LIBRARIES = android-support-v4

LOCAL_PROGUARD_FLAG_FILES := proguard.cfg

$(warning "DragonAging SW_CHIP_PLATFORM is $(SW_CHIP_PLATFORM)")
ifneq (,$(filter 9%,$(PLATFORM_VERSION)))
    $(warning "DragonAging platform_version is $(PLATFORM_VERSION)")
    LOCAL_STATIC_JAVA_LIBRARIES += libgpio softwinner.audio.static
    LOCAL_PACKAGE_NAME := DragonAgingTV
endif

include $(BUILD_PACKAGE)

# Use the following include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))

include $(CLEAR_VARS)
LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := libsysapi:libs/sysapi.jar
include $(BUILD_MULTI_PREBUILT)

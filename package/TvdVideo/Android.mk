LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_PRIVATE_PLATFORM_APIS:=true
LOCAL_PACKAGE_NAME := TvdVideo
LOCAL_CERTIFICATE := platform
#LOCAL_SDK_VERSION=current
LOCAL_PRIVATE_PLATFORM_APIS := true
include $(BUILD_PACKAGE)

# Use the folloing include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))
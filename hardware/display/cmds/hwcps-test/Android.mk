LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := hwcps-test
LOCAL_SRC_FILES := main.cpp

LOCAL_C_INCLUDES += system/core/base/include
LOCAL_C_INCLUDES += system/core/include
LOCAL_C_INCLUDES += vendor/aw/homlet/hardware/include/display

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libsysutils \
	libcutils \
	libbinder \
	liblog \
	libhwcprivateservice

# install to /vendor/bin/
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS        := optional

LOCAL_CFLAGS:= -DLOG_TAG=\"hwcps\"

include $(BUILD_EXECUTABLE)


LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := dispdebug
LOCAL_SRC_FILES := main.cpp

LOCAL_C_INCLUDES += system/core/base/include
LOCAL_C_INCLUDES += system/core/include
LOCAL_C_INCLUDES += vendor/aw/homlet/hardware/include/display
LOCAL_CFLAGS     := -DLOG_TAG=\"dispdebug\"

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libsysutils \
	libcutils \
	libbinder \
	liblog \
	softwinner.homlet.displayd@1.0 \
	libhardware \
	libhidlbase

# install to /vendor/bin/
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS        := optional

include $(BUILD_EXECUTABLE)


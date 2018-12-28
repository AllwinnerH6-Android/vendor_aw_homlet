LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := multi_ir
LOCAL_SRC_FILES := \
	multiir.c \
	virtual_input.c \
	uevent_utils.c

# install to /vendor/bin/
#LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS        := optional

LOCAL_INIT_RC := multi_ir.rc
#LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_SHARED_LIBRARIES := libcutils libc liblog
LOCAL_CFLAGS:= -DLOG_TAG=\"multiir\"
LOCAL_CFLAGS+= -DCEC_ONE_TOUCH_PLAY
LOCAL_C_INCLUDES += system/core/libutils/include

include $(BUILD_EXECUTABLE)


LOCAL_PATH:= $(call my-dir)

# Jpeg Streaming native

include $(CLEAR_VARS)

LOCAL_MODULE        := libjni_jpegstreamtv
#LOCAL_CFLAGS += -Wall
LOCAL_NDK_STL_VARIANT := c++_static

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/src \
                    external/jpeg

LOCAL_STATIC_LIBRARIES := libjpeg_static_ndk

LOCAL_SDK_VERSION   := current
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS    += -ffast-math -O3 -funroll-loops
#LOCAL_CPPFLAGS += $(JNI_CFLAGS)
LOCAL_LDLIBS := -llog

LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES     := \
    src/inputstream_wrapper.cpp \
    src/jpegstream.cpp \
    src/jerr_hook.cpp \
    src/jpeg_hook.cpp \
    src/jpeg_writer.cpp \
    src/jpeg_reader.cpp \
    src/outputstream_wrapper.cpp \
    src/stream_wrapper.cpp
LOCAL_CFLAGS += -Wall -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-implicit-function-declaration

include $(BUILD_SHARED_LIBRARY)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_STATIC_JAVA_LIBRARIES := android-support-v13
LOCAL_STATIC_JAVA_LIBRARIES += com.android.gallery3d.commonTv
LOCAL_STATIC_JAVA_LIBRARIES += xmp_toolkit
LOCAL_STATIC_JAVA_LIBRARIES += mp4parser

LOCAL_SRC_FILES := \
    $(call all-java-files-under, src) \
    $(call all-renderscript-files-under, src)
LOCAL_SRC_FILES += $(call all-java-files-under, src_pd)

LOCAL_RESOURCE_DIR += $(LOCAL_PATH)/res

LOCAL_AAPT_FLAGS := --auto-add-overlay

LOCAL_PACKAGE_NAME := GalleryTV

LOCAL_OVERRIDES_PACKAGES := Gallery Gallery3D GalleryNew3D

#LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_CFLAGS += -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-implicit-function-declaration
LOCAL_JNI_SHARED_LIBRARIES := libjni_eglfencetv libjni_filtershow_filters_tv libjni_jpegstreamtv

#LOCAL_STATIC_JAVA_LIBRARIES += \
#    android-support-test \
#    mockito-target-minus-junit4 
	
LOCAL_SDK_VERSION := current
LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_JAVA_LIBRARIES += org.apache.http.legacy

include $(BUILD_PACKAGE)

ifeq ($(strip $(LOCAL_PACKAGE_OVERRIDES)),)

# Use the following include to make gallery test apk
include $(call all-makefiles-under, $(LOCAL_PATH))

endif
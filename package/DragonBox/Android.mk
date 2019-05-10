LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_STATIC_JAVA_LIBRARIES := android-support-v13
#LOCAL_STATIC_JAVA_LIBRARIES := libsysapi
LOCAL_SRC_FILES := $(call all-java-files-under, src) \

#for A20
ifeq (A20, $(SW_CHIP_PLATFORM))
	LOCAL_SRC_FILES += $(call all-java-files-under, platform/A20)
endif
#for A10
ifeq (A10, $(SW_CHIP_PLATFORM))
	LOCAL_SRC_FILES += $(call all-java-files-under, platform/A10)
endif
#for A31
ifeq (A31, $(SW_CHIP_PLATFORM))
	LOCAL_SRC_FILES += $(call all-java-files-under, platform/A31)
endif
#for A80
ifeq (A80, $(SW_CHIP_PLATFORM))
	LOCAL_SRC_FILES += $(call all-java-files-under, platform/A80)
endif
#for H8
ifeq (H8, $(SW_CHIP_PLATFORM))
	LOCAL_SRC_FILES += $(call all-java-files-under, platform/H8)
endif

#for H3
ifeq (H3, $(SW_CHIP_PLATFORM))
    ifneq (,$(filter 4.4%,$(PLATFORM_VERSION)))
    LOCAL_SRC_FILES += $(call all-java-files-under, platform/H3-Android4.4)
    endif
    ifneq (,$(filter 7%,$(PLATFORM_VERSION)))
    LOCAL_SRC_FILES += $(call all-java-files-under, platform/H3-AndroidN)
    LOCAL_JAVA_LIBRARIES:= org.apache.http.legacy.boot
    endif
endif

ifeq (H64, $(SW_CHIP_PLATFORM))
        LOCAL_SRC_FILES += $(call all-java-files-under, platform/H64)
endif

ifeq (H5, $(SW_CHIP_PLATFORM))
    ifneq (,$(filter 4.4%,$(PLATFORM_VERSION)))
    LOCAL_SRC_FILES += $(call all-java-files-under, platform/H5-Android4.4)
    endif
    ifneq (,$(filter 7%,$(PLATFORM_VERSION)))
    LOCAL_SRC_FILES += $(call all-java-files-under, platform/H5-AndroidN)
    LOCAL_JAVA_LIBRARIES:= org.apache.http.legacy.boot
    endif
endif
ifeq (H6, $(SW_CHIP_PLATFORM))
    ifneq (,$(filter 4.4%,$(PLATFORM_VERSION)))
        ifneq (,$(filter yst-petrel_ai_p1,$(TARGET_BUSINESS_PRODUCT)))
            $(warning "TARGET_BUSINESS_PRODUCT is $(TARGET_BUSINESS_PRODUCT)!!!")
            LOCAL_SRC_FILES += $(call all-java-files-under, platform/H6-Android4.4-yst)
        else
            LOCAL_SRC_FILES += $(call all-java-files-under, platform/H6-Android4.4)
        endif
    endif
    ifneq (,$(filter 7%,$(PLATFORM_VERSION)))
    LOCAL_SRC_FILES += $(call all-java-files-under, platform/H6-AndroidN)
    LOCAL_JAVA_LIBRARIES:= org.apache.http.legacy.boot
    endif
    ifneq (,$(filter 9%,$(PLATFORM_VERSION)))
    LOCAL_SRC_FILES += $(call all-java-files-under, platform/H6-AndroidP)
    LOCAL_JAVA_LIBRARIES:= org.apache.http.legacy.boot
    LOCAL_STATIC_JAVA_LIBRARIES += libgpio softwinner.audio.static libsystemmix
    endif
endif

LOCAL_PACKAGE_NAME := DragonBox

LOCAL_REQUIRED_MODULES :=liballwinnertech_read_private_dragonbox

# We mark this out until Mtp and MediaMetadataRetriever is unhidden.
#LOCAL_SDK_VERSION := current
LOCAL_CERTIFICATE := platform

LOCAL_PROGUARD_FLAG_FILES := proguard.cfg
LOCAL_PROGUARD_ENABLED := full obfuscation

LOCAL_PRIVATE_PLATFORM_APIS := true

include $(BUILD_PACKAGE)

# Use the following include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))

#include $(CLEAR_VARS)
#LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := libsysapi:libs/sysapi.jar
#include $(BUILD_MULTI_PREBUILT)
$(warning $(PLATFORM_HARDWARE))
$(warning $(PLATFORM_SDK_VERSION))

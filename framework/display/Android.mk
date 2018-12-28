LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := softwinner.display

LOCAL_SRC_FILES += \
      $(call all-java-files-under,java)

LOCAL_STATIC_JAVA_LIBRARIES := \
    android.hidl.manager-V1.0-java \
    softwinner.homlet.displayd-V1.0-java

include $(BUILD_STATIC_JAVA_LIBRARY)

# Generate DisplayDaemon.jar
include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(call all-java-files-under, DisplayDaemon)
LOCAL_STATIC_JAVA_LIBRARIES += softwinner.display
LOCAL_MODULE := DisplayDaemon
LOCAL_MODULE_STEM := DisplayDaemon
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/service/Android.mk


#app
PRODUCT_PACKAGES += \
    TvdVideo \
    TvLauncher \
    TvdFileManager \
    SettingsAssist \
    MiracastReceiver \
    WebScreensaver \
    DragonBox \
    DragonAgingTV \
    GalleryTV \
    #DragonSN  \

#service
PRODUCT_PACKAGES += \
    libsystemmix_jni \
    systemmixservice \
    libsystemmixservice \
    libisomountmanager_jni \
    libisomountmanagerservice \
    libsystemmix \
    nfsprobe \
    libjni_swos \
    libadmanager_jni \
    libjni_WFDManager.so \
    libwfdrtsp.so \
    libwfdplayer.so \
    libwfdmanager.so

#utils
PRODUCT_PACKAGES += \
   multi_ir \
   qw

#add debug method for eng img
ifeq ($(TARGET_BUILD_VARIANT),eng)
PRODUCT_PROPERTY_OVERRIDES += \
    vold.debug=true
endif

SUNXI_VENDOR_KL_DIR := vendor/aw/homlet/hardware/input/multi_ir/keylayout
PRODUCT_COPY_FILES += \
    $(SUNXI_VENDOR_KL_DIR)/virtual-remote.kl:system/usr/keylayout/virtual-remote.kl \
    $(SUNXI_VENDOR_KL_DIR)/sunxi-ir.kl:system/usr/keylayout/sunxi-ir.kl \
    $(SUNXI_VENDOR_KL_DIR)/customer_ir_9f00.kl:system/usr/keylayout/customer_ir_9f00.kl \
    $(SUNXI_VENDOR_KL_DIR)/customer_ir_dd22.kl:system/usr/keylayout/customer_ir_dd22.kl \
    $(SUNXI_VENDOR_KL_DIR)/customer_ir_fb04.kl:system/usr/keylayout/customer_ir_fb04.kl \
    $(SUNXI_VENDOR_KL_DIR)/customer_ir_ff00.kl:system/usr/keylayout/customer_ir_ff00.kl \
    $(SUNXI_VENDOR_KL_DIR)/customer_ir_4cb3.kl:system/usr/keylayout/customer_ir_4cb3.kl \
    $(SUNXI_VENDOR_KL_DIR)/customer_ir_bc00.kl:system/usr/keylayout/customer_ir_bc00.kl \
    $(SUNXI_VENDOR_KL_DIR)/customer_ir_fc00.kl:system/usr/keylayout/customer_ir_fc00.kl \
    $(SUNXI_VENDOR_KL_DIR)/customer_ir_2992.kl:system/usr/keylayout/customer_ir_2992.kl \
	$(SUNXI_VENDOR_KL_DIR)/customer_ir_fe01.kl:system/usr/keylayout/customer_ir_fe01.kl \
    $(SUNXI_VENDOR_KL_DIR)/customer_rc5_ir_04.kl:system/usr/keylayout/customer_rc5_ir_04.kl \
    $(SUNXI_VENDOR_KL_DIR)/sunxi-ir-uinput.kl:system/usr/keylayout/sunxi-ir-uinput.kl \

# for gpio
PRODUCT_PACKAGES += \
    gpioservice \
    libgpio_jni \
    libgpioservice \

# for homlet display hal
PRODUCT_PACKAGES += \
    libhwcprivateservice \
    libedid \
    libdisplayd \
    softwinner.homlet.displayd@1.0 \
    softwinner.homlet.displayd@1.0-service

PRODUCT_PACKAGES += \
    displayd-test \
    softwinner.display \
    DisplayDaemon \
    DisplayDaemonService \
    ubootparam \
    dispdebug
# secure storage support
PRODUCT_PACKAGES += \
    libsst \
    sst_test_v2

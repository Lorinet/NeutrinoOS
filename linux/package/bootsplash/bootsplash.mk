BOOTSPLASH_SITE=$(BR2_EXTERNAL_LinfinityLinux_PATH)/package/bootsplash/source
BOOTSPLASH_SITE_METHOD=local
BOOTSPLASH_INSTALL_TARGET=YES

define BOOTSPLASH_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/bootsplash $(TARGET_DIR)/sbin/bootsplash
endef

define BOOTSPLASH_INSTALL_INIT_SYSV
    $(INSTALL) -D -m 0755 $(BR2_EXTERNAL_LinfinityLinux_PATH)/package/bootsplash/S01bootsplash $(TARGET_DIR)/etc/init.d/S01bootsplash
endef

$(eval $(cmake-package))

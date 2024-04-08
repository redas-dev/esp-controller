include $(TOPDIR)/rules.mk

PKG_NAME:=ESPController
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/ESPController
	CATEGORY:=Base system
	TITLE:=ESPController
	DEPENDS:= +libubus +libubox +cJSON +libserialport +libblobmsg-json
endef

define Package/ESPController/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/etc/init.d

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ESPController $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/
	$(INSTALL_BIN) ./files/ESPController.init $(1)/etc/init.d/ESPController
endef

$(eval $(call BuildPackage,ESPController))
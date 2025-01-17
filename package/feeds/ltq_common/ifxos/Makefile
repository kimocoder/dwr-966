#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ifxos
PKG_BASE_NAME:=lib_ifxos
PKG_VERSION:=1.5.19
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://localhost
PKG_MD5SUM:=b5c502608ea3bdfde182a620d627bb3f

PKG_FIXUP:=libtool
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package-version-override.mk
include $(INCLUDE_DIR)/package.mk

ifeq ($(DUMP),)
	STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell $(SH_FUNC) grep '^CONFIG_PACKAGE_.*ifxos' $(TOPDIR)/.config | md5s)
endif

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Lantiq
  SUBMENU:=IFXOS
  TITLE:=Lantiq OS abstraction library (Linux)
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq
  MENU:=1
endef

define Package/$(PKG_NAME)/config
  $(call Package/$(PKG_NAME)/override_source_path)
endef

define Package/$(PKG_NAME)/description
  This library will be used by some Lantiq applications / drivers under Linux
endef

define KernelPackage/$(PKG_NAME)
  SUBMENU:=Lantiq
  TITLE:=Lantiq OS abstraction library
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq
  FILES:=$(PKG_INSTALL_DIR)/usr/bin/drv_ifxos$(PKG_MODULE_EXTENSION)
  AUTOLOAD:=$(call AutoLoad,10,drv_ifxos)
endef

MAKE_FLAGS+=-s

ifneq ($(CONFIG_PACKAGE_kmod-ifxos),)
CONFIGURE_ARGS += --with-kernel-module
CONFIGURE_ARGS += --enable-kernelincl="$(LINUX_DIR)/include"

ifeq ($(CONFIG_LINUX_2_4),)
CONFIGURE_ARGS += --enable-linux-26 --enable-kernelbuild="$(LINUX_DIR)" ARCH=$(LINUX_KARCH)
PKG_MODULE_EXTENSION:=.ko
endif

else
CONFIGURE_ARGS += --without-kernel-module
endif

ifeq ($(CONFIG_LINUX_2_4),y)

ifneq ($(CONFIG_mips)$(CONFIG_mipsel),)
CONFIGURE_ARGS += --enable-add_drv_cflags="-fno-pic -mno-abicalls -mlong-calls -G 0"
endif

define KernelPackage/$(PKG_NAME)/install
	cd $(1)/lib/modules/$(LINUX_VERSION) && ln -snf drv_ifxos drv_ifxos.o
endef

endif

define Build/Clean
	@- rm -Rf $(PKG_BUILD_DIR)/ipkg*
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include/ifxos}
	$(CP) $(PKG_BUILD_DIR)/src/include/* $(1)/usr/include/ifxos

	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libifxos.a $(1)/usr/lib/libifxos.a

endef

define Build/UninstallDev
	- rm -Rf $(1)/usr/include/ifxos
	- rm -f $(1)/usr/lib/libifxos*
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
$(eval $(call KernelPackage,$(PKG_NAME)))

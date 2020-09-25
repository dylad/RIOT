PKG_NAME    = rpmsg-lite
PKG_URL     = https://github.com/NXPmicro/rpmsg-lite
PKG_VERSION = cb56a06c66dd7d85e72a9cc7e8e5bf3c8773df6b
PKG_LICENSE = BSD-3-Clause

PKG_BUILDDIR ?= $(PKGDIRBASE)/rpmsg-lite

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/lib/rpmsg_lite -f $(CURDIR)/Makefile.rpmsg-lite
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/lib/virtio -f $(CURDIR)/Makefile.rpmsg-lite.virtio
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/lib/common -f $(CURDIR)/Makefile.rpmsg-lite.common
	
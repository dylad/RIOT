PKG_NAME=mpfs_sdk
PKG_URL=https://github.com/polarfire-soc/platform
PKG_VERSION=13c8cc74e445008bf8615097d44ea28af4ff8d97
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

ifneq ($(CPU),polarfire)
  $(error This package can only be used with MPFS CPUs)
endif

MPFS_MODULES = mpfs_sdk mpfs_peripherals

DIR_mpfs_peripherals = mpfs_hal/common
DIR_mpfs_sdk = drivers/mss/mss_mmuart

.PHONY = mpfs_%

all: $(MPFS_MODULES)

mpfs_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(DIR_$@) -f $(CURDIR)/Makefile.$@


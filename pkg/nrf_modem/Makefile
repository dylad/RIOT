PKG_NAME=nrf_modem
PKG_URL=https://github.com/nrfconnect/sdk-nrfxlib
PKG_VERSION=fa5a790a287954915b4e3e48a4ff3cf614d04a2a
PKG_LICENSE=-Nordic-5-Clause

include $(RIOTBASE)/pkg/pkg.mk


MODEM_LIB_PATH = nrf_modem/lib/cortex-m33/soft-float
PKG_BUILD_DIR = $(BUILD_DIR)/pkg/$(PKG_NAME)

.PHONY: all

all: $(BINDIR)/libmodem.a


$(BINDIR)/libmodem.a: $(PKG_BUILD_DIR)
	@cp $(PKG_BUILD_DIR)/$(MODEM_LIB_PATH)/libmodem.a $@
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(PKG_NAME)/lib -f $(CURDIR)/Makefile.nrf_modem

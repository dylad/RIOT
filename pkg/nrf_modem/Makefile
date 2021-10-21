PKG_NAME=nrf_modem
PKG_URL=https://github.com/nrfconnect/sdk-nrfxlib
PKG_VERSION=9c49c779a2c938dd6a4a2e7192f4ff1d9c32f346
PKG_LICENSE=-Nordic-5-Clause

include $(RIOTBASE)/pkg/pkg.mk

MODEM_LIB_PATH = nrf_modem/lib/cortex-m33/soft-float
PKG_BUILD_DIR = $(BUILD_DIR)/pkg/$(PKG_NAME)

.PHONY: all

all: $(BINDIR)/libmodem.a

download_fw:$(PKG_BUILD_DIR)
	$(Q)$(DOWNLOAD_TO_FILE) $(PKG_BUILD_DIR)/firmware.zip $(PKG_FW_URL)/$(PKG_FW_NAME)_$(PKG_FW_VERSION).zip

#unzip_fw: $(PKG_BUILD_DIR)/firmware.zip
#	$(Q)$(UNZIP_HERE) -D -d $(PKG_BUILD_DIR)/firmware $<

$(BINDIR)/libmodem.a: $(PKG_BUILD_DIR)
	@cp $(PKG_BUILD_DIR)/$(MODEM_LIB_PATH)/libmodem.a $@
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(PKG_NAME)/lib -f $(CURDIR)/Makefile.nrf_modem

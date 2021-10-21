
#include download firmware information from nrf_modem pkg
include $(RIOTPKG)/nrf_modem/nrf_modem_fw.mk

FIRMWARE = $(BUILD_DIR)/pkg/nrf_modem/firmware.zip
nrf_modem/flash_firmware:
	$(info Download & flash nRF modem firmware to you board...)
	$(info this may takes a while)
#check if zip archive already exist, otherwise download it
	$(Q)if [ -f "$(FIRMWARE)" ]; then \
		echo File exists, skip download; \
	else \
		$(DOWNLOAD_TO_FILE) $(FIRMWARE) $(PKG_FW_URL)/$(PKG_FW_NAME)_$(PKG_FW_VERSION).zip; \
	fi
	$(Q)$(FLASHER) --program $(FIRMWARE) --verify

NRFJPROG ?= nrfjprog
FLASHER ?= $(NRFJPROG)
#nrfjprog only support hexfile
FLASHFILE = $(HEXFILE)
FFLAGS ?= --program $(FLASHFILE)

# Give the possibility to user to flash nRF modem firmware
# To do so, use make nrf_mode/flash_firmware
ifneq (,$(filter nrf_modem, $(USEPKG)))
  include $(RIOTMAKE)/nrf_modem_fw.inc.mk
endif
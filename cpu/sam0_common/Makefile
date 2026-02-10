ifneq (,$(filter mchp_periph_ng, $(USEMODULE)))
  DIRS = periph_ng
else
  DIRS = periph
endif

ifneq (,$(filter mchp_dfp_hal,$(USEMODULE)))
	DIRS += mchp_dfp_hal
endif

ifneq (,$(filter sam0_eth, $(USEMODULE)))
  DIRS += sam0_eth
endif
ifneq (,$(filter sam0_sdhc, $(USEMODULE)))
  DIRS += sam0_sdhc
endif

include $(RIOTBASE)/Makefile.base

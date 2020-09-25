MODULE = rpmsg-lite-contrib

SRC = rpmsg_env_riot.c
INCLUDES += -I$(RIOTBASE)/pkg/rpmsg-lite/include \
            -I$(PKGDIRBASE)/rpmsg-lite/lib/include
## This package can only be used on supported platforms
ifeq (imx8-m4,$(CPU))
SRC += rpmsg_platform.c
else
$(error Selected CPU is not supported or compatible)
endif
include $(RIOTBASE)/Makefile.base

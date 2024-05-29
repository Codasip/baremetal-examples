# ----[ PATHS ]----

PLATFORM_DIR := $(subst /platform.mk,,$(lastword $(MAKEFILE_LIST)))

# ----[ LDSCRIPT ]----

LDFLAGS += -Wl,-L$(PLATFORM_DIR)
CFLAGS  += -I$(PLATFORM_DIR)

# ----[ PROVIDES ]----

ifneq ($(CONFIG_ENVIRONMENT),SIMULATOR)
PROVIDES += clint
PROVIDES += flash
PROVIDES += gpio_io
PROVIDES += sdcard
PROVIDES += plic
PROVIDES += uart
ifeq ($(CONFIG_SECURITY),Y)
PROVIDES += aead
PROVIDES += trng
endif
endif

# ----[ LIB SOURCES ]----

ifneq ($(CONFIG_ENVIRONMENT),SIMULATOR)
BM_SOURCES += \
    $(PLATFORM_DIR)/platform.c \
    $(LIB_DIR)/src/clint.c \
    $(LIB_DIR)/src/gpio.c \
    $(LIB_DIR)/src/plic.c \
    $(LIB_DIR)/src/spi.c \
    $(LIB_DIR)/src/uart.c
ifeq ($(CONFIG_SECURITY),Y)
BM_SOURCES += \
    $(LIB_DIR)/src/aead.c \
    $(LIB_DIR)/src/trng.c
endif
endif

# ----[ DEFINES ]----

ifeq ($(CONFIG_SECURITY),Y)
DEFINES += CONFIG_SECURITY
endif

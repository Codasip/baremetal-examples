# ----[ PATHS ]----

PLATFORM_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# ----[ VARIABLES ]----

CFLAGS  += -I$(PLATFORM_DIR)

# ----[ PROVIDES ]----

ifneq ($(CONFIG_ENVIRONMENT),SIMULATOR)
PROVIDES += clint
PROVIDES += flash
PROVIDES += gpio_io
PROVIDES += sdcard
PROVIDES += id_registers
ifeq ($(CONFIG_PLIC),Y)
PROVIDES += plic
endif
PROVIDES += uart
ifeq ($(CONFIG_SECURITY),Y)
PROVIDES += aead
PROVIDES += mailbox
PROVIDES += trng
endif
endif

# ----[ LIB SOURCES ]----

BM_SOURCES += \
    $(PLATFORM_DIR)/platform.c

ifneq ($(CONFIG_ENVIRONMENT),SIMULATOR)
BM_SOURCES += \
    $(LIB_DIR)/src/clint.c \
    $(LIB_DIR)/src/gpio.c \
    $(LIB_DIR)/src/id_reg.c \
    $(LIB_DIR)/src/spi.c \
    $(LIB_DIR)/src/uart.c
ifeq ($(CONFIG_PLIC),Y)
BM_SOURCES += \
    $(LIB_DIR)/src/plic.c
endif
ifeq ($(CONFIG_SECURITY),Y)
BM_SOURCES += \
    $(LIB_DIR)/src/aead.c \
    $(LIB_DIR)/src/trng.c
endif
endif

# ----[ DEFINES ]----

ifeq ($(CONFIG_PLIC),Y)
DEFINES += CONFIG_PLIC
endif

ifeq ($(CONFIG_SECURITY),Y)
DEFINES += CONFIG_SECURITY
endif

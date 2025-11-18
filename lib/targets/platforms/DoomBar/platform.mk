# ----[ PATHS ]----

PLATFORM_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# ----[ VARIABLES ]----

CFLAGS  += -I$(PLATFORM_DIR)

# ----[ PROVIDES ]----

ifneq ($(CONFIG_ENVIRONMENT),SIMULATOR)
PROVIDES += clint
PROVIDES += gpio_io
PROVIDES += flash
PROVIDES += sdcard
PROVIDES += trng
PROVIDES += uart
PROVIDES += i2c_pwr
PROVIDES += mailbox
PROVIDES += aead
endif

# ----[ LIB SOURCES ]----

BM_SOURCES += \
    $(PLATFORM_DIR)/platform.c

ifneq ($(CONFIG_ENVIRONMENT),SIMULATOR)
BM_SOURCES += \
    $(LIB_DIR)/src/aead.c \
    $(LIB_DIR)/src/clint.c \
    $(LIB_DIR)/src/gpio.c \
    $(LIB_DIR)/src/i2c.c \
    $(LIB_DIR)/src/spi.c \
    $(LIB_DIR)/src/trng.c \
    $(LIB_DIR)/src/uart.c
endif

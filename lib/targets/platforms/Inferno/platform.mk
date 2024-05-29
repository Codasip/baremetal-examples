# ----[ PATHS ]----

PLATFORM_DIR := $(subst /platform.mk,,$(lastword $(MAKEFILE_LIST)))

# ----[ LDSCRIPT ]----

LDFLAGS += -Wl,-L$(PLATFORM_DIR)
CFLAGS  += -I$(PLATFORM_DIR)

# ----[ PROVIDES ]----

ifneq ($(CONFIG_ENVIRONMENT),SIMULATOR)
PROVIDES += clint
PROVIDES += gpio_io
PROVIDES += sdcard
PROVIDES += uart
endif

# ----[ LIB SOURCES ]----

ifneq ($(CONFIG_ENVIRONMENT),SIMULATOR)
BM_SOURCES += \
    $(PLATFORM_DIR)/platform.c \
    $(LIB_DIR)/src/clint.c \
    $(LIB_DIR)/src/gpio.c \
    $(LIB_DIR)/src/spi.c \
    $(LIB_DIR)/src/uart.c
endif

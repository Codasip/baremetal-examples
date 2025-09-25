# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += trng
REQUIRES += sdcard
CPPFLAGS += "-DDATA_LEN=1048576"

APP     = trng-demo
SOURCES = \
	$(DEMO_DIR)/src/trng-demo.c \
	$(LIB_DIR)/fatfs/diskio.c \
	$(LIB_DIR)/fatfs/ff.c \
	$(LIB_DIR)/fatfs/ffunicode.c \
	$(LIB_DIR)/fatfs/mmc.c

CPPFLAGS += -I $(DEMO_DIR)/src

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

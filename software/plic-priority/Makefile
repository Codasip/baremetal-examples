# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += sdcard
REQUIRES += gpio_io
REQUIRES += plic

APP     = plic-priority
SOURCES = $(DEMO_DIR)/src/plic-priority.c

include $(DEMO_DIR)/../../share/app.mk

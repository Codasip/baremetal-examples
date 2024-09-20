# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += sdcard
REQUIRES += gpio_io
REQUIRES += clic

APP     = clic-priority
SOURCES = $(DEMO_DIR)/src/clic-priority.c

include $(DEMO_DIR)/../../share/app.mk

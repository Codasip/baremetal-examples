# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += gpio_io

APP     = oob-demo
SOURCES = $(DEMO_DIR)/src/oob-demo.c

include $(DEMO_DIR)/../../share/app.mk

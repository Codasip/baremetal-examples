# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += gpio_io

APP     = gpio-interrupt-demo
SOURCES = $(DEMO_DIR)/src/gpio-interrupt-demo.c

include $(DEMO_DIR)/../../share/app.mk

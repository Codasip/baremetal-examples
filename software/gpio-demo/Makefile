# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += gpio_io

APP     = gpio-demo
SOURCES = $(DEMO_DIR)/src/gpio-demo.c

include $(DEMO_DIR)/../../share/app.mk

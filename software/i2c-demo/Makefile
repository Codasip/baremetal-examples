# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += i2c_pwr

APP     = i2c-demo
SOURCES = $(DEMO_DIR)/src/i2c-demo.c

include $(DEMO_DIR)/../../share/app.mk

# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += clint

APP     = wfi-demo
SOURCES = $(DEMO_DIR)/src/wfi-demo.c


include $(DEMO_DIR)/../../share/app.mk

# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += uart

APP     = uart-demo
SOURCES = $(DEMO_DIR)/src/uart-demo.c


include $(DEMO_DIR)/../../share/app.mk

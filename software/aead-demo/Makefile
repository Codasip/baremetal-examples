# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += aead

APP     = aead-demo
SOURCES = $(DEMO_DIR)/src/aead-demo.c

include $(DEMO_DIR)/../../share/app.mk

# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += l31cache

APP     = cache-info-demo
SOURCES = $(DEMO_DIR)/src/cache-info-demo.c

include $(DEMO_DIR)/../../share/app.mk

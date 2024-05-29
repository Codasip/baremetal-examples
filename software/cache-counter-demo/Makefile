# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += l31cache

APP     = cache-counter-demo
SOURCES = $(DEMO_DIR)/src/cache-counter-demo.c

include $(DEMO_DIR)/../../share/app.mk

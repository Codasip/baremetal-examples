# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += pmp
REQUIRES += user_mode

APP     = pmp-demo
SOURCES = $(DEMO_DIR)/src/pmp-demo.c

include $(DEMO_DIR)/../../share/app.mk

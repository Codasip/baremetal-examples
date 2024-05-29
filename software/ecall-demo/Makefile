# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += user_mode

APP     = ecall-demo
SOURCES = $(DEMO_DIR)/src/ecall-demo.c

include $(DEMO_DIR)/../../share/app.mk

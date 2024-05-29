# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += clint

APP     = clint-ipi
SOURCES = $(DEMO_DIR)/src/clint-ipi.c

include $(DEMO_DIR)/../../share/app.mk

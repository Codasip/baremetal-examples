# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += clint

APP     = clint-timer
SOURCES = $(DEMO_DIR)/src/clint-timer.c

include $(DEMO_DIR)/../../share/app.mk

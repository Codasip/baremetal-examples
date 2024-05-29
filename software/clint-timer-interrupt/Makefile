# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += clint

APP     = clint-timer-interrupt
SOURCES = $(DEMO_DIR)/src/clint-timer-interrupt.c

include $(DEMO_DIR)/../../share/app.mk

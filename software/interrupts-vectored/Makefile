# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += clint

APP     = interrupts-vectored
SOURCES = $(DEMO_DIR)/src/interrupts-vectored.c

include $(DEMO_DIR)/../../share/app.mk

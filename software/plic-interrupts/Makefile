# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += gpio_io
REQUIRES += plic

APP     = plic-interrupts
SOURCES = $(DEMO_DIR)/src/plic-interrupts.c

include $(DEMO_DIR)/../../share/app.mk

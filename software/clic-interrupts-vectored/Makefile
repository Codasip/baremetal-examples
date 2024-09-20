# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += clic

APP     = clic-interrupts-vectored
SOURCES = $(DEMO_DIR)/src/clic-interrupts-vectored.c

include $(DEMO_DIR)/../../share/app.mk

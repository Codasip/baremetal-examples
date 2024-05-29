# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += pic

APP     = pic-interrupts
SOURCES = $(DEMO_DIR)/src/pic-interrupts.c

include $(DEMO_DIR)/../../share/app.mk

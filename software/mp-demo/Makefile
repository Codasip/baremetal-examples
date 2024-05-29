# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += atomics

APP     = mp-demo
SOURCES = $(DEMO_DIR)/src/mp-demo.c

include $(DEMO_DIR)/../../share/app.mk

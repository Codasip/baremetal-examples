# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += clint
REQUIRES += user_mode

APP     = rdtime
SOURCES = $(DEMO_DIR)/src/rdtime.c

include $(DEMO_DIR)/../../share/app.mk

CFLAGS  +=-O2

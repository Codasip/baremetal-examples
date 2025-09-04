# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += sdcard
REQUIRES += gpio_io
REQUIRES += clic

APP     = clic-priority
SOURCES = $(DEMO_DIR)/src/clic-priority.c

include $(BM_SDK_DIR)/share/app.mk

# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += clic
REQUIRES += gpio_io

APP     = clic-interrupts-vectored
SOURCES = $(DEMO_DIR)/src/clic-interrupts-vectored.c

include $(BM_SDK_DIR)/share/app.mk

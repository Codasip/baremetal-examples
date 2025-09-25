# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += gpio_io
REQUIRES += plic

APP     = plic-interrupts
SOURCES = $(DEMO_DIR)/src/plic-interrupts.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

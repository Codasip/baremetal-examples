# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += clic
REQUIRES += gpio_io

APP     = clic-interrupts
SOURCES = $(DEMO_DIR)/src/clic-interrupts.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

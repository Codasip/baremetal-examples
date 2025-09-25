# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += gpio_io

APP     = oob-demo
SOURCES = $(DEMO_DIR)/src/oob-demo.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

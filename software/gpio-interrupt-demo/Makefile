# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += gpio_io

APP     = gpio-interrupt-demo
SOURCES = $(DEMO_DIR)/src/gpio-interrupt-demo.c

include $(BM_SDK_DIR)/share/app.mk

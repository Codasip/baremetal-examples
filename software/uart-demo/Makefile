# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += uart

APP     = uart-demo
SOURCES = $(DEMO_DIR)/src/uart-demo.c


include $(BM_SDK_DIR)/share/app.mk

# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

APP     = interrupts-simple
SOURCES = $(DEMO_DIR)/src/interrupts-simple.c

include $(BM_SDK_DIR)/share/app.mk

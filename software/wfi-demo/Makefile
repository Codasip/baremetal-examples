# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += clint

APP     = wfi-demo
SOURCES = $(DEMO_DIR)/src/wfi-demo.c


include $(BM_SDK_DIR)/share/app.mk

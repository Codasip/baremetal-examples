# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += i2c_pwr

APP     = i2c-demo
SOURCES = $(DEMO_DIR)/src/i2c-demo.c

include $(BM_SDK_DIR)/share/app.mk

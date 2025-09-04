# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += aead

APP     = aead-demo
SOURCES = $(DEMO_DIR)/src/aead-demo.c

include $(BM_SDK_DIR)/share/app.mk

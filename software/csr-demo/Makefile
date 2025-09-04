# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

APP     = csr-demo
SOURCES = $(DEMO_DIR)/src/csr-demo.c

include $(BM_SDK_DIR)/share/app.mk

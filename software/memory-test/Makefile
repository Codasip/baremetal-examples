# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

APP     = memory-test
SOURCES = $(DEMO_DIR)/src/memory-test.c

include $(BM_SDK_DIR)/share/app.mk

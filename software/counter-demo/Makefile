# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

APP     = counter-demo
SOURCES = $(DEMO_DIR)/src/counter-demo.c

include $(BM_SDK_DIR)/share/app.mk

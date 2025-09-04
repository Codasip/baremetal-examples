# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += id_registers

APP     = id-registers-demo
SOURCES = $(DEMO_DIR)/src/id-registers-demo.c

include $(BM_SDK_DIR)/share/app.mk

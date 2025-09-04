# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += atomics

APP     = mp-demo
SOURCES = $(DEMO_DIR)/src/mp-demo.c

include $(BM_SDK_DIR)/share/app.mk

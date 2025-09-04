# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += pmp
REQUIRES += user_mode

APP     = pmp-demo
SOURCES = $(DEMO_DIR)/src/pmp-demo.c

include $(BM_SDK_DIR)/share/app.mk

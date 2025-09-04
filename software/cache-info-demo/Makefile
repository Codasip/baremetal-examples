# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += l31cache

APP     = cache-info-demo
SOURCES = $(DEMO_DIR)/src/cache-info-demo.c

include $(BM_SDK_DIR)/share/app.mk

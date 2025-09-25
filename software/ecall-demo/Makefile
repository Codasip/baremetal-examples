# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += user_mode

APP     = ecall-demo
SOURCES = $(DEMO_DIR)/src/ecall-demo.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

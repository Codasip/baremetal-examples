# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += tcm

APP     = tcm-demo
SOURCES = $(DEMO_DIR)/src/tcm-demo.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

ifneq ($(CC_TYPE),riscv_gcc)
CFLAGS += -mlong-calls
endif

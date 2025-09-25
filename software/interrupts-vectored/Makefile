# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += clint pic_or_plic

APP     = interrupts-vectored
SOURCES = $(DEMO_DIR)/src/interrupts-vectored.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += clint

APP     = clint-timer-interrupt
SOURCES = $(DEMO_DIR)/src/clint-timer-interrupt.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

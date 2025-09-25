# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += clint

APP     = clint-timer
SOURCES = $(DEMO_DIR)/src/clint-timer.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

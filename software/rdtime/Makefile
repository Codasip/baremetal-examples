# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += clint
REQUIRES += user_mode

APP     = rdtime
SOURCES = $(DEMO_DIR)/src/rdtime.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror -O2

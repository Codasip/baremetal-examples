# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += pic

APP     = pic-interrupts
SOURCES = $(DEMO_DIR)/src/pic-interrupts.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

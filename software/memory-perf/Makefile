# Baremetal SDK application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

APP     = memory-perf
SOURCES = $ \
	$(DEMO_DIR)/src/memory.c \
	$(DEMO_DIR)/src/memory-perf.c

CFLAGS  = -O1

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

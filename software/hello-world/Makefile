# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

APP     = hello-world
SOURCES = $(DEMO_DIR)/src/hello-world.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

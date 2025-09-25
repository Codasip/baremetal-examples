# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += clint
REQUIRES += user_mode
REQUIRES += supervisor_mode

APP     = privilege-interrupts-delegated
SOURCES = $(DEMO_DIR)/src/privilege-interrupts-delegated.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

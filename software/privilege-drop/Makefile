# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += user_mode

APP     = privilege-drop
SOURCES = $(DEMO_DIR)/src/privilege-drop.c

include $(BM_SDK_DIR)/share/app.mk

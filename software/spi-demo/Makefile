# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += flash

APP     = spi-demo
SOURCES = $(DEMO_DIR)/src/spi-demo.c \
          $(DEMO_DIR)/src/s25fl128s.c


include $(BM_SDK_DIR)/share/app.mk

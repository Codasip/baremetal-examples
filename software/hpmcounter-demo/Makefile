# Baremetal application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

REQUIRES += hpm

APP     = hpmcounter-demo
SOURCES = $(DEMO_DIR)/src/hpmcounter-demo.c

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

ifeq ($(CONFIG_PROCESSOR),L31)
CPPFLAGS += -DTEST_HPM_EVENT=BM_HPM_LOAD_STALLS
else ifeq ($(CONFIG_PROCESSOR),A730)
CPPFLAGS += -DTEST_HPM_EVENT=BM_HPM_ZERO_INSTR_ISSUED
else ifeq ($(CONFIG_PROCESSOR),X730)
CPPFLAGS += -DTEST_HPM_EVENT=BM_HPM_ZERO_INSTR_ISSUED
else ifeq ($(CONFIG_PROCESSOR),L730)
CPPFLAGS += -DTEST_HPM_EVENT=BM_HPM_ZERO_INSTR_ISSUED
endif

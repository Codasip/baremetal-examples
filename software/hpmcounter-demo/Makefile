# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += hpm

APP     = hpmcounter-demo
SOURCES = $(DEMO_DIR)/src/hpmcounter-demo.c

include $(DEMO_DIR)/../../share/app.mk

ifeq ($(CONFIG_PROCESSOR),L31)
CPPFLAGS += -DTEST_HPM_EVENT=BM_HPM_LOAD_STALLS
else ifeq ($(CONFIG_PROCESSOR),A730)
CPPFLAGS += -DTEST_HPM_EVENT=BM_HPM_ZERO_INSTR_ISSUED
endif
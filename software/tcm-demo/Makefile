# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += tcm

APP     = tcm-demo
SOURCES = $(DEMO_DIR)/src/tcm-demo.c

include $(DEMO_DIR)/../../share/app.mk

ifneq ($(CC_TYPE),riscv_gcc)
CFLAGS += -mlong-calls
endif

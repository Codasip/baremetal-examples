# Baremetal Examples Top-Level Makefile

TOP_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

ifdef CC
  ifndef CC_NAME
    CC_NAME := $(CC)
  endif
  ifndef SDK_PREFIX
    SDK_PREFIX := $(shell realpath $(dir $(shell command -v $(firstword $(CC_NAME)))))/
  endif
endif

ifdef CONFIG_TARGET
CONFIG_FILE ?=  $(TOP_DIR)/lib/targets/configs/config-$(CONFIG_TARGET).mk
else
CONFIG_FILE ?=  $(TOP_DIR)/config.mk
endif

ifeq ("$(wildcard $(CONFIG_FILE))","")
    $(error "missing CONFIG_FILE: '$(CONFIG_FILE)'")
endif
include ${CONFIG_FILE}

ifeq ($(DEMO_APP),)
$(error DEMO_APP must be defined)
endif

include $(TOP_DIR)/software/$(DEMO_APP)/Makefile

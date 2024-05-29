# Baremetal Examples Top-Level Makefile

TOP_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

CONFIG_FILE ?= config.mk
include ${CONFIG_FILE}

ifeq ($(DEMO_APP),)
$(error DEMO_APP must be defined)
endif

include $(TOP_DIR)/software/$(DEMO_APP)/Makefile

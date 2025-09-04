CWD := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BASE_CONFIG := $(CWD)/config-l110-eagle.mk

# base configuration
include $(BASE_CONFIG)

# add/overwrite settings
CONFIG_CBI_ENABLE           = 1
CONFIG_CBI_MASK             = 0x2

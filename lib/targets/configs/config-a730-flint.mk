# ----[ INCLUDE DEFAULT CONFIGURATION ]----

include $(dir $(lastword $(MAKEFILE_LIST)))/config-a730-default.mk

# ----[ CORE CONFIGURATION DIFFERENCES ]----

CONFIG_CORE_FREQ            = 100
CONFIG_NUM_HARTS            = 4

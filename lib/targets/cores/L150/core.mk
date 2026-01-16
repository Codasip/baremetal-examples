# ----[ PATHS ]----

CORE_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# ----[ VARIABLES ]----

CFLAGS   += -I$(CORE_DIR)
ASFLAGS  += -I$(CORE_DIR)

# ----[ Basic Core Configuration ]----

XLEN                := 32

CONFIG_HAS_EXT_I    := Y
CONFIG_HAS_EXT_M    := Y
CONFIG_HAS_EXT_C    := Y

CONFIG_EXT_Z        += zicsr
CONFIG_EXT_Z        += zifencei

ifeq ($(CC_TYPE), codasip_clang)
# Only define ARCH/ABI for non-Codasip compilers, a Codasip SDK defaults to the
# correct setting for the associated core.
CONFIG_CC_USE_DEFAULT_ARCH ?= Y
CONFIG_CC_USE_DEFAULT_ABI  ?= Y
endif

# ----[ SIMULATOR CONFIGURATION ]----
CONFIG_SIM_HALT_ADDR ?= 0x6f010000

# ----[ DEFINES ]----

DEFINES += CONFIG_CORE_FREQ=$(CONFIG_CORE_FREQ)000000
DEFINES += CONFIG_CLIC_NUM_INTERRUPTS=$(CONFIG_CLIC_NUM_INTERRUPTS)
DEFINES += CONFIG_CLIC_INTCTLBITS=$(CONFIG_CLIC_INTCTLBITS)

ifeq ($(CONFIG_ENVIRONMENT),SIMULATOR)
DEFINES += CONFIG_SIM_HALT_ADDR=$(CONFIG_SIM_HALT_ADDR)
endif

ifdef CONFIG_CBI_ENABLE
DEFINES += CONFIG_CBI_ENABLE=$(CONFIG_CBI_ENABLE)
DEFINES += CONFIG_CBI_MASK=$(CONFIG_CBI_MASK)
endif

# ----[ PROVIDES ]----

PROVIDES += clic

# ----[ LIB SOURCES ]----

BM_CRT0 += \
    $(CORE_DIR)/core_init.S

BM_SOURCES += \
    $(LIB_DIR)/src/clic.c

# ----[ PRINTOUTS ]----

$(info Core Configuration:)
$(info - Frequency       : $(CONFIG_CORE_FREQ))
$(info - CLIC interrupts : $(CONFIG_CLIC_NUM_INTERRUPTS))
ifdef CONFIG_CBI_ENABLE
$(info - CBI enable      : $(CONFIG_CBI_ENABLE))
endif
$(info )

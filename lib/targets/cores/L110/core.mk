# ----[ PATHS ]----

CORE_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# ----[ VARIABLES ]----

MARCH := rv32imc_zicsr_zifencei
MABI  := ilp32
XLEN  := 32

ifneq ($(CC_TYPE), codasip_clang)
# Only define ARCH and ABI for non-Codasip compilers,
# a Codasip SDK defaults to the correct ARCH & ABI for the associated core
CPPFLAGS += -march=$(MARCH) -mabi=$(MABI)
endif

CFLAGS   += -I$(CORE_DIR)
ASFLAGS  += -I$(CORE_DIR)

# ----[ SIMULATOR CONFIGURATION ]----
CONFIG_SIM_HALT_ADDR ?= 0x6f010000

# ----[ LIB SOURCES ]----

BM_CRT0 += \
    $(CORE_DIR)/core_init.S

BM_SOURCES += \
    $(CORE_DIR)/target_csr.c \
    $(LIB_DIR)/src/clic.c

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

# ----[ PRINTOUTS ]----

$(info Core Configuration:)
$(info - Frequency       : $(CONFIG_CORE_FREQ))
$(info - CLIC interrupts : $(CONFIG_CLIC_NUM_INTERRUPTS))
ifdef CONFIG_CBI_ENABLE
$(info - CBI enable      : $(CONFIG_CBI_ENABLE))
endif
$(info )

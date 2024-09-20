# ----[ PATHS ]----

CORE_DIR := $(subst /core.mk,,$(lastword $(MAKEFILE_LIST)))

# ----[ VARIABLES ]----

MARCH := rv32imc
MABI  := ilp32
XLEN  := 32

MARCH :=$(MARCH)_zicsr_zifencei

ifneq ($(CC_TYPE),codasip_clang)
CPPFLAGS += -march=$(MARCH) -mabi=$(MABI)
endif
CFLAGS   += -I$(CORE_DIR)
ASFLAGS  += -I$(CORE_DIR)

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

# ----[ PROVIDES ]----

PROVIDES += clic

# ----[ PRINTOUTS ]----

$(info Core Configuration:)
$(info - Frequency       : $(CONFIG_CORE_FREQ))
$(info - CLIC interrupts : $(CONFIG_CLIC_NUM_INTERRUPTS))
$(info )


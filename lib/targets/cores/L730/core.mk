# ----[ PATHS ]----

CORE_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# ----[ VARIABLES ]----
ifeq ($(CONFIG_HAS_FPU_DP),Y)
# EXT-D
MARCH := rv32imafdc_zicsr_zifencei_zba_zbb_zbs_zicbom_zicboz
MABI  := ilp32d
else
MARCH := rv32imafc_zicsr_zifencei_zba_zbb_zbs_zicbom_zicboz
MABI  := ilp32f
endif

XLEN  := 32

ifneq ($(CC_TYPE), codasip_clang)
# Only define ARCH and ABI for non-Codasip compilers,
# a Codasip SDK defaults to the correct ARCH & ABI for the associated core
CPPFLAGS += -march=$(MARCH) -mabi=$(MABI)
endif

CFLAGS   += -I$(CORE_DIR)
ASFLAGS  += -I$(CORE_DIR)
LDFLAGS  += -Wl,--defsym=_NUM_HARTS=$(CONFIG_NUM_HARTS)

# ----[ DEFINES ]----

DEFINES += CONFIG_CORE_FREQ=$(CONFIG_CORE_FREQ)000000
DEFINES += CONFIG_NUM_HARTS=$(CONFIG_NUM_HARTS)
DEFINES += CONFIG_CLIC_NUM_INTERRUPTS=$(CONFIG_CLIC_NUM_INTERRUPTS)
DEFINES += CONFIG_CLIC_INTCTLBITS=$(CONFIG_CLIC_INTCTLBITS)

ifeq ($(CONFIG_HAS_FPU),Y)
DEFINES += CONFIG_HAS_FPU
endif

ifeq ($(CONFIG_HAS_PMP),Y)
DEFINES += CONFIG_HAS_PMP
endif

ifeq ($(CONFIG_HAS_CACHES),Y)
DEFINES += CONFIG_HAS_CACHES
endif

ifeq ($(CONFIG_HAS_TCMS),Y)
DEFINES += CONFIG_HAS_TCMS
endif

# ----[ PROVIDES ]----

PROVIDES += atomics
PROVIDES += hpm
PROVIDES += supervisor_mode
PROVIDES += user_mode
PROVIDES += clic

ifeq ($(CONFIG_HAS_CACHES),Y)
#PROVIDES += ToDo
endif

ifeq ($(CONFIG_HAS_PMP),Y)
PROVIDES += pmp
endif

ifeq ($(CONFIG_HAS_TCMS),Y)
PROVIDES += tcm
endif

ifeq ($(CONFIG_HAS_HPM),Y)
PROVIDES += hpm
endif

# ----[ LIB SOURCES ]----

BM_CRT0 += \
    $(CORE_DIR)/core_init.S

BM_SOURCES += \
    $(CORE_DIR)/target_csr.c \
    $(CORE_DIR)/target_hpm.c \
    $(LIB_DIR)/src/clic.c

ifeq ($(CONFIG_HAS_PMP),Y)
BM_SOURCES += $(LIB_DIR)/src/pmp.c
endif

# ----[ PRINTOUTS ]----

$(info Core Configuration:)
$(info - Frequency       : $(CONFIG_CORE_FREQ))
$(info - Harts           : $(CONFIG_NUM_HARTS))
$(info - FPU             : $(CONFIG_HAS_FPU))
$(info - PMP             : $(CONFIG_HAS_PMP))
ifeq ($(CONFIG_HAS_PMP),Y)
$(info - PMP regions     : $(CONFIG_PMP_NUM_REGIONS))
endif
$(info - TCM             : $(CONFIG_HAS_TCMS))
$(info - Cache control   : $(CONFIG_HAS_CACHES))
$(info - Number of CLIC interrupts : $(CONFIG_CLIC_NUM_INTERRUPTS))
$(info )

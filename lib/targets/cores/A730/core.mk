# ----[ PATHS ]----

CORE_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# ----[ VARIABLES ]----

CFLAGS   += -I$(CORE_DIR)
ASFLAGS  += -I$(CORE_DIR)

# ----[ Basic Core Configuration ]----

XLEN                := 64

CONFIG_HAS_EXT_I    := Y
CONFIG_HAS_EXT_M    := Y
CONFIG_HAS_EXT_A    := Y
CONFIG_HAS_FPU      := Y
CONFIG_HAS_FPU_DP   := Y
CONFIG_HAS_EXT_C    := Y

CONFIG_HAS_EXT_S    := Y
CONFIG_HAS_EXT_U    := Y

CONFIG_EXT_Z        += zicsr
CONFIG_EXT_Z        += zifencei
CONFIG_EXT_Z        += zba
CONFIG_EXT_Z        += zbb
CONFIG_EXT_Z        += zbs
CONFIG_EXT_Z        += zicbom
CONFIG_EXT_Z        += zicboz

ifeq ($(CC_TYPE), codasip_clang)
# Only define ARCH/ABI for non-Codasip compilers, a Codasip SDK defaults to the
# correct setting for the associated core.
CONFIG_CC_USE_DEFAULT_ARCH := Y
CONFIG_CC_USE_DEFAULT_ABI  := Y
endif

# ----[ DEFINES ]----

DEFINES += CONFIG_CORE_FREQ=$(CONFIG_CORE_FREQ)000000
DEFINES += CONFIG_NUM_HARTS=$(CONFIG_NUM_HARTS)

ifeq ($(CONFIG_HAS_PMP),Y)
DEFINES += CONFIG_HAS_PMP
DEFINES += CONFIG_PMP_NUM_REGIONS=$(CONFIG_PMP_NUM_REGIONS)
endif

ifeq ($(CONFIG_HAS_TCMS),Y)
DEFINES += CONFIG_HAS_TCMS
endif

# ----[ PROVIDES ]----

PROVIDES += atomics
PROVIDES += hpm
PROVIDES += supervisor_mode
PROVIDES += user_mode

ifeq ($(CONFIG_HAS_PMP),Y)
PROVIDES += pmp
endif

# ----[ LIB SOURCES ]----

BM_CRT0 += \
    $(CORE_DIR)/core_init.S

BM_SOURCES += \
    $(CORE_DIR)/target_hpm.c

ifeq ($(CONFIG_HAS_PMP),Y)
BM_SOURCES += $(LIB_DIR)/src/pmp.c
endif

# ----[ PRINTOUTS ]----

$(info Core Configuration:)
$(info - Frequency       : $(CONFIG_CORE_FREQ))
$(info - Harts           : $(CONFIG_NUM_HARTS))
$(info - PMP             : $(CONFIG_HAS_PMP))
$(info - TCM             : $(CONFIG_HAS_TCMS))
$(info )

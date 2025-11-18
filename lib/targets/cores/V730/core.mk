# ----[ PATHS ]----

CORE_DIR := $(subst /core.mk,,$(lastword $(MAKEFILE_LIST)))

# ----[ VARIABLES ]----

CFLAGS   += -I$(CORE_DIR)
ASFLAGS  += -I$(CORE_DIR)

# ----[ Basic Core Configuration ]----

XLEN                := 32

CONFIG_HAS_CHERI    := Y

CONFIG_HAS_EXT_I    := Y
CONFIG_HAS_EXT_M    := Y
CONFIG_HAS_EXT_A    := Y
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
ifeq ($(CONFIG_HAS_FPU),Y)
CONFIG_EXT_Z        += zfhmin
endif

ifeq ($(CC_TYPE), codasip_clang)
# Only define ARCH/ABI for non-Codasip compilers, a Codasip SDK defaults to the
# correct setting for the associated core.
CONFIG_CC_USE_DEFAULT_ARCH := Y
CONFIG_CC_USE_DEFAULT_ABI  := Y
endif

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
DEFINES += CONFIG_PMP_NUM_REGIONS=$(CONFIG_PMP_NUM_REGIONS)
endif

ifeq ($(CONFIG_HAS_CACHES),Y)
DEFINES += CONFIG_HAS_CACHES
endif

ifeq ($(CONFIG_HAS_TCMS),Y)
DEFINES += CONFIG_HAS_TCMS
endif

ifeq ($(CONFIG_HAS_CHERI),Y)
PROVIDES += cheri
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
$(info - CHERI           : $(CONFIG_HAS_CHERI))
$(info )

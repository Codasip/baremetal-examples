# ----[ PATHS ]----

CORE_DIR := $(subst /core.mk,,$(lastword $(MAKEFILE_LIST)))

# ----[ VARIABLES ]----

MARCH := rv32imc
MABI  := ilp32
XLEN  := 32

ifeq ($(CONFIG_HAS_FPU),Y)
MARCH := rv32imfc
MABI  := ilp32f
endif

MARCH :=$(MARCH)_zicsr_zifencei

CPPFLAGS += -march=$(MARCH) -mabi=$(MABI)
CFLAGS   += -I$(CORE_DIR)
ASFLAGS  += -I$(CORE_DIR)

# ----[ DEFINES ]----

DEFINES += CONFIG_CORE_FREQ=$(CONFIG_CORE_FREQ)000000

ifeq ($(CONFIG_HAS_FPU),Y)
DEFINES += CONFIG_HAS_FPU
endif

DEFINES += CONFIG_PIC_NUM_INTERRUPTS=$(CONFIG_PIC_NUM_INTERRUPTS)

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

# ----[ PROVIDES ]----

PROVIDES += pic
PROVIDES += pic_or_plic
PROVIDES += user_mode

ifeq ($(CONFIG_HAS_CACHES),Y)
PROVIDES += l31cache
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

BM_SOURCES += \
    $(CORE_DIR)/target_csr.c \
    $(LIB_DIR)/src/pic.c

ifeq ($(CONFIG_HAS_HPM),Y)
BM_SOURCES += $(CORE_DIR)/target_hpm.c
endif

ifeq ($(CONFIG_HAS_PMP),Y)
BM_SOURCES += $(LIB_DIR)/src/pmp.c
endif

# ----[ PRINTOUTS ]----

$(info Core Configuration:)
$(info - Frequency       : $(CONFIG_CORE_FREQ))
$(info - FPU             : $(CONFIG_HAS_FPU))
$(info - PMP             : $(CONFIG_HAS_PMP))
ifeq ($(CONFIG_HAS_PMP),Y)
$(info - PMP regions     : $(CONFIG_PMP_NUM_REGIONS))
endif
$(info - TCM             : $(CONFIG_HAS_TCMS))
$(info - Cache control   : $(CONFIG_HAS_CACHES))
$(info - Number of PIC interrupts : $(CONFIG_PIC_NUM_INTERRUPTS))
$(info )


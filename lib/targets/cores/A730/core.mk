# ----[ PATHS ]----

CORE_DIR := $(subst /core.mk,,$(lastword $(MAKEFILE_LIST)))

# ----[ VARIABLES ]----

MARCH := rv64imafdc_zicsr_zifencei_zba_zbb_zbs_zicbom_zicboz
MABI  := lp64d
XLEN  := 64

CPPFLAGS += -march=$(MARCH) -mabi=$(MABI)
CFLAGS   += -I$(CORE_DIR)
ASFLAGS  += -I$(CORE_DIR)
LDFLAGS  += -Wl,--defsym=_NUM_HARTS=$(CONFIG_NUM_HARTS)

# ----[ DEFINES ]----

DEFINES += CONFIG_CORE_FREQ=$(CONFIG_CORE_FREQ)000000
DEFINES += CONFIG_NUM_HARTS=$(CONFIG_NUM_HARTS)

ifeq ($(CONFIG_HAS_PMP),Y)
DEFINES += CONFIG_HAS_PMP
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
    $(CORE_DIR)/target_csr.c \
    $(CORE_DIR)/target_hpm.c

ifeq ($(CONFIG_HAS_PMP),Y)
BM_SOURCES += $(LIB_DIR)/src/pmp.c
endif

# ----[ PRINTOUTS ]----

$(info Core Configuration:)
$(info - Frequency       : $(CONFIG_CORE_FREQ))
$(info - Harts           : $(CONFIG_NUM_HARTS))
$(info - PMP             : $(CONFIG_HAS_PMP))
$(info )


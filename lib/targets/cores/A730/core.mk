# ----[ PATHS ]----

CORE_DIR := $(subst /core.mk,,$(lastword $(MAKEFILE_LIST)))

# ----[ VARIABLES ]----

# For A730 (None-CHERI) we should use rv64imafdc_zicsr_zifencei_zba_zbb_zbs_zcb_zfhmin
# Trimmed for backwards compatability
# MARCH := rv64imafdc_zicsr_zifencei_zba_zbb_zbs_zcb_zfhmin
ifeq ($(findstring codasip-,$(COMPILER_VERSION_STRING)),obilix-)
# "obilix-" detected
MARCH := rv64imafdc_zicsr_zifencei_zba_zbb_zbs_zcb_zfhmin

else ifeq ($(findstring codasip-,$(COMPILER_VERSION_STRING)),codasip-)
# "codasip-" detected
MARCH := rv64imafdc_zicsr_zifencei_zba_zbb_zbs_zcb_zfhmin

else ifeq ($(findstring clang,$(COMPILER_VERSION_STRING)),clang)
# "clang" detected
MARCH := rv64imafdc_zicsr_zifencei_zba_zbb_zbs_zcb_zfhmin

else ifeq ($(findstring gcc,$(COMPILER_VERSION_STRING)),gcc)
# "gcc" detected
MARCH := rv64imafdc_zicsr_zifencei_zba_zbb_zbs_zicbom_zicboz

else
$(error Unrecognised compiler version "$(COMPILER_VERSION_STRING)")
endif
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
$(info - TCM             : $(CONFIG_HAS_TCMS))
$(info )

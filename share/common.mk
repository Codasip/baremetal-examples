#
# Common Makefile for the Bare-metal examples
# Responsibility:
# - parse config.mk and set proper variables
# - detect and set compiler
# - export variables needed for the build

# ----[ Helpers ]----

empty :=
space := $(empty) $(empty)

# ----[ DEFAULTS ]----

CONFIG_ENVIRONMENT ?= FPGA_UART

# ----[ TOOLCHAIN ]----

OS_SUFFIX =
CHECK_HAVE_TOOL = $(if $(shell which $(1)),Y,N)
ifeq ($(OS),Windows_NT)
OS_SUFFIX = .exe
CHECK_HAVE_TOOL = $(if $(shell where $(1)),Y,N)
endif

ifeq ($(SDK_PREFIX),)
$(warning "SDK_PREFIX" variable is not set)
endif

DETECTED_PREFIX =
ifneq ($(CC_NAME),)
COMPILER_VERSION_STRING := $(shell $(SDK_PREFIX)$(CC_NAME) --version)
ifneq ($(COMPILER_VERSION_STRING),)
DETECTED_PREFIX = $(SDK_PREFIX)
else
COMPILER_VERSION_STRING := $(shell $(SDK_PREFIX)-$(CC_NAME) --version)
ifneq ($(COMPILER_VERSION_STRING),)
DETECTED_PREFIX = $(SDK_PREFIX)-
endif
endif
endif
ifeq ($(DETECTED_PREFIX),)
COMPILER_VERSION_STRING := $(shell $(SDK_PREFIX)gcc --version)
ifneq ($(COMPILER_VERSION_STRING),)
DETECTED_PREFIX = $(SDK_PREFIX)
CC_NAME = gcc
else
COMPILER_VERSION_STRING := $(shell $(SDK_PREFIX)-gcc --version)
ifneq ($(COMPILER_VERSION_STRING),)
DETECTED_PREFIX = $(SDK_PREFIX)-
CC_NAME = gcc
else
COMPILER_VERSION_STRING := $(shell $(SDK_PREFIX)clang --version)
ifneq ($(COMPILER_VERSION_STRING),)
DETECTED_PREFIX = $(SDK_PREFIX)
CC_NAME = clang
else
COMPILER_VERSION_STRING := $(shell $(SDK_PREFIX)-clang --version)
ifneq ($(COMPILER_VERSION_STRING),)
DETECTED_PREFIX = $(SDK_PREFIX)-
CC_NAME = clang
endif
endif
endif
endif
endif

ifeq ($(DETECTED_PREFIX),)
$(error No compiler recognized with "$(SDK_PREFIX)" prefix, fix prefix in "SDK_PREFIX" variable or set correct compiler name in "CC_NAME" variable)
endif

CC := $(DETECTED_PREFIX)$(CC_NAME)$(OS_SUFFIX)

ifneq ($(findstring bakewell,$(COMPILER_VERSION_STRING)),)
# Found "Bakewell" which is the Codasip CHERI SDK (e.g. codasip-embedded-sdk-1.1.0)
OBJCOPY ?= $(DETECTED_PREFIX)llvm-objcopy$(OS_SUFFIX)
OBJDUMP ?= $(DETECTED_PREFIX)llvm-objdump$(OS_SUFFIX)
CC_TYPE = riscv_clang
LD_TARGET = riscv

# Force -march and -mabi on to the compiler command line
CONFIG_CC_USE_DEFAULT_ARCH ?= N
CONFIG_CC_USE_DEFAULT_ABI  ?= N

# Use Codasip CHERI/Non-CHERI SDK Lib Gloss and startup files.
# The SDK startup code, for all secondary cores, calls __main() (or wfi, if __main() does not exist).
USE_SDK_GLOSS ?= Y

else ifneq ($(findstring codasip-,$(COMPILER_VERSION_STRING)),)
OBJCOPY ?= $(DETECTED_PREFIX)llvm-objcopy$(OS_SUFFIX)
OBJDUMP ?= $(DETECTED_PREFIX)llvm-objdump$(OS_SUFFIX)
CC_TYPE = codasip_clang
ifeq ($(findstring Target: codasip-,$(COMPILER_VERSION_STRING)),Target: codasip-)
LD_TARGET = codasip
else
LD_TARGET = riscv
endif
SIM ?= $(DETECTED_PREFIX)isimulator$(OS_SUFFIX)

else ifneq ($(findstring clang,$(COMPILER_VERSION_STRING)),)
OBJCOPY ?= $(DETECTED_PREFIX)objcopy$(OS_SUFFIX)
OBJDUMP ?= $(DETECTED_PREFIX)objdump$(OS_SUFFIX)
CC_TYPE = riscv_clang
LD_TARGET = riscv

else ifneq ($(findstring gcc,$(COMPILER_VERSION_STRING)),)
OBJCOPY ?= $(DETECTED_PREFIX)objcopy$(OS_SUFFIX)
OBJDUMP ?= $(DETECTED_PREFIX)objdump$(OS_SUFFIX)
CC_TYPE = riscv_gcc
LD_TARGET = riscv

else
$(error Unrecognised compiler version "$(COMPILER_VERSION_STRING)")
endif

# ----[ PATHS ]----

THIS_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
TOP_DIR  := $(THIS_DIR)/..
LIB_DIR  := $(TOP_DIR)/lib

# ----[ VERSIONING ]----

VERSION := $(strip $(file < $(TOP_DIR)/VERSION))
ifeq ($(VERSION),)
VERSION=Unknown
endif

COMMIT=
ifeq ($(call CHECK_HAVE_TOOL,git),Y)
# Get the curret commit ID. If the folder is no repo at all, this sets an error
# code, a warning on stderr, and nothing on stdout. So just discarding stderr.
COMMIT := $(shell git -C $(TOP_DIR) rev-parse --short HEAD 2>/dev/null)
endif
ifeq ($(COMMIT),)
COMMIT=Unknown
endif

# ----[ CONFIG MAKE INCLUDE ]----
ifdef CONFIG_TARGET
CONFIG_FILE ?=  $(TOP_DIR)/lib/targets/configs/config-$(CONFIG_TARGET).mk
else
CONFIG_FILE ?=  $(TOP_DIR)/config.mk
endif

ifeq ("$(wildcard $(CONFIG_FILE))","")
    $(error "missing CONFIG_FILE: '$(CONFIG_FILE)'")
endif
include $(CONFIG_FILE)

# ----[ PRINTOUTS ]----

$(info )
$(info BareMetal Examples Build Configuration)
$(info - Application     : $(APP))
$(info - Version         : $(VERSION))
$(info - Commit          : $(COMMIT))
$(info )
$(info Toolchain Configuration:)
$(info - Compiler        : $(CC))
$(info - Linker target   : $(LD_TARGET))
$(info )
$(info Target Configuration:)
$(info - Processor       : $(CONFIG_PROCESSOR))
$(info - Platform        : $(CONFIG_PLATFORM))
$(info - Syscalls        : $(CONFIG_ENVIRONMENT))
$(info )

# ----[ TARGET MAKE INCLUDES ]----

CORE_CONFIG_FILE ?= $(LIB_DIR)/targets/cores/$(CONFIG_PROCESSOR)/core.mk
ifeq ("$(wildcard $(CORE_CONFIG_FILE))","")
    $(error "missing CORE_CONFIG_FILE: '$(CORE_CONFIG_FILE)'")
endif
include $(CORE_CONFIG_FILE)

PLATFORM_CONFIG_FILE ?= $(LIB_DIR)/targets/platforms/$(CONFIG_PLATFORM)/platform.mk
ifeq ("$(wildcard $(PLATFORM_CONFIG_FILE))","")
    $(error "missing PLATFORM_CONFIG_FILE: '$(PLATFORM_CONFIG_FILE)'")
endif
include $(PLATFORM_CONFIG_FILE)

# ----[ COMPILER CONFIGURATION: ARCH]----

CC_ARCH_ITEMS :=

ifeq ($(CONFIG_HAS_EXT_I),Y)
CC_ARCH_ITEMS += i
else ifeq ($(CONFIG_HAS_EXT_E),Y)
CC_ARCH_ITEM += e
else
# ToDo: Support CONFIG_HAS_EXT_Y for CHERI.
#       Note: currently supported in X730 and V730 core.mk as "CONFIG_EXT_Z += zcherihybrid"
$(error Unknown RISC-V architecture)
endif

ifeq ($(CONFIG_HAS_EXT_M),Y)
CC_ARCH_ITEMS += m
endif

ifeq ($(CONFIG_HAS_EXT_A),Y)
CC_ARCH_ITEMS += a
endif

ifeq ($(CONFIG_HAS_FPU),Y)
CC_ARCH_ITEMS += f
ifeq ($(CONFIG_HAS_FPU_DP),Y)
CC_ARCH_ITEMS += d
endif
endif

ifeq ($(CONFIG_HAS_EXT_C),Y)
CC_ARCH_ITEMS += c
endif

# Use MARCH if this is set explicitly. Otherwise set it using the string build
# from the config - unless this is explicitly disabled.
CONFIG_CC_ARCH := rv$(XLEN)$(subst $(space),,$(CC_ARCH_ITEMS))$(subst $(space),,$(addprefix _,$(CONFIG_EXT_Z)))
ifneq ($(CONFIG_CC_USE_DEFAULT_ARCH),Y)
MARCH ?= $(CONFIG_CC_ARCH)
endif
ifneq ($(MARCH),)
CPPFLAGS += -march=$(MARCH)
endif

# ----[ COMPILER CONFIGURATION: ABI]----

CC_ABI_ITEMS :=

ifeq ($(XLEN),32)
CC_ABI_ITEMS += i
endif

ifeq ($(CONFIG_HAS_CHERI),Y)
# Add Cheri specific ABI nomenclature
ifeq ($(XLEN),32)
CC_ABI_ITEMS += l32pc64
else ifeq ($(XLEN),64)
CC_ABI_ITEMS += l64pc128
endif

else
# Add non-Cheri ABI nomenclature
CC_ABI_ITEMS += lp$(XLEN)
endif

# Add float/double ABI nomenclature
ifeq ($(CONFIG_HAS_FPU),Y)
ifeq ($(CONFIG_HAS_FPU_DP),Y)
CC_ABI_ITEMS += d
else
CC_ABI_ITEMS += f
endif
endif

# Use MABI if this is set explicitly. Otherwise set it using the string build
# from the config - unless this is explicitly disabled.
CONFIG_CC_ABI := $(subst $(space),,$(CC_ABI_ITEMS))
ifneq ($(CONFIG_CC_USE_DEFAULT_ABI),Y)
MABI ?= $(CONFIG_CC_ABI)
endif
ifneq ($(MABI),)
CPPFLAGS += -mabi=$(MABI)
endif

# ----[ PRINTOUTS ]----

$(info Compiler Configuration:)
ifneq ($(MARCH),$(CONFIG_CC_ARCH))
$(info - CONFIG_CC_ARCH  : $(CONFIG_CC_ARCH))
endif
$(info - ARCH            : $(if $(MARCH),$(MARCH),(toolchain default)))

ifneq ($(MABI),$(CONFIG_CC_ABI))
$(info - CONFIG_CC_ABI   : $(CONFIG_CC_ABI))
endif
$(info - ABI             : $(if $(MABI),$(MABI),(toolchain default)))

# ----[ LDSCRIPT ]----

ifneq ($(USE_SDK_GLOSS),Y)
LDSCRIPT ?= $(LD_TARGET)$(XLEN).ld
LDFLAGS += -Wl,-L$(CORE_DIR)
LDFLAGS += -Wl,-L$(PLATFORM_DIR)
LDFLAGS += -Wl,-L$(LIB_DIR)/linker
LDFLAGS += -Wl,--defsym=_STACK_SIZE=0x4000
LDFLAGS += -Wl,--defsym=_HEAP_SIZE=0x4000

else

# Use Codasip CHERI/Non-CHERI SDK Lib Gloss and startup files
ifeq ($(CONFIG_ENVIRONMENT),FPGA_SEMIHOSTING)
LDSCRIPT ?= baremetal-hobgoblin2-sram.ld

else ifeq ($(CONFIG_ENVIRONMENT),SIMULATOR)
ifeq ($(CONFIG_PROCESSOR),L31)
$(error Unsupported CONFIG_ENVIRONMENT value with Cheri Bakewell SDK: '$(CONFIG_ENVIRONMENT)')
else
LDSCRIPT ?= baremetal-coretb.ld
endif

else ifeq ($(CONFIG_ENVIRONMENT),FPGA_UART)
LDSCRIPT ?= baremetal-hobgoblin2-sram.ld

else
$(error Unsupported CONFIG_ENVIRONMENT value: '$(CONFIG_ENVIRONMENT)')
endif

endif

ifdef CONFIG_NUM_HARTS
LDFLAGS += -Wl,--defsym=_NUM_HARTS=$(CONFIG_NUM_HARTS)
endif

# ----[ INCLUDES ]----

CPPFLAGS += -I $(LIB_DIR)
CPPFLAGS += -I $(LIB_DIR)/include
CPPFLAGS += -I $(LIB_DIR)/targets

# ----[ SYSCALLS ]----

ifneq ($(USE_SDK_GLOSS),Y)

BM_SOURCES += \
    $(LIB_DIR)/syscalls/sys_sbrk.c \
    $(LIB_DIR)/syscalls/sys_exit.c \
    $(LIB_DIR)/syscalls/sys_empty.c

ifeq ($(CONFIG_ENVIRONMENT),FPGA_SEMIHOSTING)
BM_SOURCES += $(LIB_DIR)/syscalls/sys_semihost.c
else ifeq ($(CONFIG_ENVIRONMENT),SIMULATOR)
ifeq ($(CONFIG_PROCESSOR),L31)
BM_SOURCES += $(LIB_DIR)/syscalls/sys_nexus.c
else
BM_SOURCES += $(LIB_DIR)/syscalls/sys_semihost.c
endif
else ifeq ($(CONFIG_ENVIRONMENT),FPGA_UART)
BM_SOURCES += $(LIB_DIR)/syscalls/sys_uart.c
else
$(error Unsupported CONFIG_ENVIRONMENT value: '$(CONFIG_ENVIRONMENT)')
endif


else
# Use Cheri/Non-Cheri Bakewell SDK Lib Gloss and startup files
CPPFLAGS += -mcmodel=medany

ifeq ($(CONFIG_ENVIRONMENT),FPGA_SEMIHOSTING)
CPPFLAGS += -moslib=gloss-semihost

else ifeq ($(CONFIG_ENVIRONMENT),SIMULATOR)
ifeq ($(CONFIG_PROCESSOR),L31)
$(error Unsupported CONFIG_ENVIRONMENT value with CHERI SDK: '$(CONFIG_ENVIRONMENT)')
else
CPPFLAGS += -moslib=gloss-coretb
endif

else ifeq ($(CONFIG_ENVIRONMENT),FPGA_UART)
CPPFLAGS += -moslib=gloss-hobgoblin

else
$(error Unsupported CONFIG_ENVIRONMENT value: '$(CONFIG_ENVIRONMENT)')
endif

endif

# ----[ CRT0 ]----

ifneq ($(USE_SDK_GLOSS),Y)
BM_CRT0 += $(LIB_DIR)/startup/crt0.S
endif

# ----[ LIB SOURCES ]----

BM_SOURCES += \
    $(LIB_DIR)/src/barrier.c \
    $(LIB_DIR)/src/counter.c \
    $(LIB_DIR)/src/id_reg.c \
    $(LIB_DIR)/src/interrupt.c \
    $(LIB_DIR)/src/interrupt_low.c \
    $(LIB_DIR)/src/mp.c \
    $(LIB_DIR)/src/mutex.c \
    $(LIB_DIR)/src/priv.c \
    $(LIB_DIR)/src/printf.c

# ----[ DEFINES ]----

DEFINES += RISCV_XLEN=$(XLEN)

ifeq ($(CONFIG_ENVIRONMENT),FPGA_SEMIHOSTING)
DEFINES += TARGET_SEMIHOSTING
else ifeq ($(CONFIG_ENVIRONMENT),SIMULATOR)
DEFINES += TARGET_SIMULATION
else ifeq ($(CONFIG_ENVIRONMENT),FPGA_UART)
DEFINES += TARGET_UART
else
$(error Unsupported CONFIG_ENVIRONMENT value: '$(CONFIG_ENVIRONMENT)')
endif

DEFINES += BUILD_VERSION=\"$(VERSION)\"
DEFINES += BUILD_ID=\"$(COMMIT)\"

CPPFLAGS += $(addprefix -D,$(DEFINES))

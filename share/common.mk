#
# Common Makefile for the Bare-metal examples
# Responsibility:
# - parse config.mk and set proper variables
# - detect and set compiler
# - export variables needed for the build

# ----[ TOOLCHAIN ]----

OS_SUFFIX =
ifeq ($(OS),Windows_NT)
OS_SUFFIX = .exe
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
else
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

CC = $(DETECTED_PREFIX)$(CC_NAME)$(OS_SUFFIX)

ifneq ($(findstring codasip,$(COMPILER_VERSION_STRING)),)
OBJCOPY ?= $(DETECTED_PREFIX)llvm-objcopy$(OS_SUFFIX)
CC_TYPE = codasip_clang
LD_TARGET = codasip
SIM ?= $(DETECTED_PREFIX)isimulator$(OS_SUFFIX)

else ifneq ($(findstring clang,$(COMPILER_VERSION_STRING)),)
OBJCOPY ?= $(DETECTED_PREFIX)objcopy$(OS_SUFFIX)
CC_TYPE = riscv_clang
LD_TARGET = riscv

else ifneq ($(findstring gcc,$(COMPILER_VERSION_STRING)),)
OBJCOPY ?= $(DETECTED_PREFIX)objcopy$(OS_SUFFIX)
CC_TYPE = riscv_gcc
LD_TARGET = riscv

else
$(error Unrecognised compiler version "$(COMPILER_VERSION_STRING)")
endif

# ----[ PATHS ]----

THIS_DIR := $(subst /common.mk,,$(lastword $(MAKEFILE_LIST)))
TOP_DIR  := $(subst /share,,$(THIS_DIR))
LIB_DIR  := $(TOP_DIR)/lib

# ----[ VERSIONING ]----

VERSION := $(strip $(file < $(TOP_DIR)/VERSION))
ifeq ($(VERSION),)
VERSION=Unknown
endif

COMMIT := $(shell git -C $(TOP_DIR) rev-parse --short HEAD)
ifeq ($(COMMIT),)
COMMIT=Unknown
endif

# ----[ CONFIG MAKE INCLUDE ]----

CONFIG_FILE ?= $(TOP_DIR)/config.mk
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
include $(CORE_CONFIG_FILE)

PLATFORM_CONFIG_FILE ?= $(LIB_DIR)/targets/platforms/$(CONFIG_PLATFORM)/platform.mk
include $(PLATFORM_CONFIG_FILE)

# ----[ LDSCRIPT ]----

LDSCRIPT ?= $(LD_TARGET)$(XLEN).ld
LDFLAGS += -Wl,-L$(LIB_DIR)/linker
LDFLAGS += -Wl,--defsym=_STACK_SIZE=0x4000 -Wl,--defsym=_HEAP_SIZE=0x4000

# ----[ INCLUDES ]----

CPPFLAGS += -I $(LIB_DIR)/include

# ----[ SYSCALLS ]----

BM_SOURCES += $(LIB_DIR)/syscalls/sys_sbrk.c \
               $(LIB_DIR)/syscalls/sys_empty.c

ifeq ($(CONFIG_PROCESSOR).$(CONFIG_ENVIRONMENT),L110.SIMULATOR)
BM_SOURCES += $(LIB_DIR)/syscalls/sys_semihost.c
else ifeq ($(CONFIG_ENVIRONMENT),FPGA_SEMIHOSTING)
BM_SOURCES += $(LIB_DIR)/syscalls/sys_semihost.c
else ifeq ($(CONFIG_ENVIRONMENT),SIMULATOR)
BM_SOURCES += $(LIB_DIR)/syscalls/sys_nexus.c
else ifeq ($(CONFIG_ENVIRONMENT),FPGA_UART)
BM_SOURCES += $(LIB_DIR)/syscalls/sys_uart.c
else
$(error Unknown CONFIG_ENVIRONMENT value)
endif

# ----[ CRT0 ]----

BM_CRT0 += $(LIB_DIR)/startup/crt0.S

# ----[ LIB SOURCES ]----

BM_SOURCES += \
    $(LIB_DIR)/src/barrier.c \
    $(LIB_DIR)/src/counter.c \
    $(LIB_DIR)/src/csr.c \
    $(LIB_DIR)/src/interrupt.c \
    $(LIB_DIR)/src/interrupt_low.c \
    $(LIB_DIR)/src/mp.c \
    $(LIB_DIR)/src/mutex.c \
    $(LIB_DIR)/src/priv.c \
    $(LIB_DIR)/src/printf.c

# ----[ DEFINES ]----

ifeq ($(CONFIG_ENVIRONMENT),SIMULATOR)
DEFINES += TARGET_SIMULATION
endif

DEFINES += BUILD_VERSION=\"$(VERSION)\"
DEFINES += BUILD_ID=\"$(COMMIT)\"

CPPFLAGS += $(addprefix -D,$(DEFINES))

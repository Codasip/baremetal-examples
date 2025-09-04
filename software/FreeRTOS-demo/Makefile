# Baremetal FreeRTOS application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

BM_SDK_DIR ?= $(DEMO_DIR)/../..

# To build this demo you need the Codasip port of FreeRTOS repo:
#   https://github.com/Codasip/FreeRTOS-Kernel.git
#
# This demo has been tested with FreeRTOS V11.2.0

# Check RTOS_DIR is set
$(if $(RTOS_DIR),,$(error missing RTOS_DIR, must point to the FreeRTOS Kernel repo))

REQUIRES += uart gpio_io

# After configuring baremetal-examples/config.mk and adding environment variable SDK_PREFIX
# cd to this directory and make


# FreeRTOS Directories
RTOS_SRC_DIR = $(RTOS_DIR)
RTOS_ASM_DIR = $(RTOS_SRC_DIR)/portable/GCC/RISC-V

# Demo App and FreeRTOS Include Directories
CPPFLAGS += -I $(LIB_DIR)/include                      \
            -I $(LIB_DIR)/targets/platforms            \
            -I $(DEMO_DIR)/src                         \
            -I $(DEMO_DIR)/src/FreeRTOS                \
            -I $(RTOS_SRC_DIR)/include                 \
            -I $(RTOS_SRC_DIR)/portable/GCC/RISC-V     \
            -I $(RTOS_SRC_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RISCV_MTIME_CLINT_CLIC_no_extensions

APP       = FreeRTOS-demo
SOURCES   = $(DEMO_DIR)/src/FreeRTOS-demo.c            \
            $(DEMO_DIR)/src/main_blinky.c              \
            $(DEMO_DIR)/src/FreeRTOS/vector.S

RTOS_SRCS = $(RTOS_SRC_DIR)/event_groups.c             \
            $(RTOS_SRC_DIR)/list.c                     \
            $(RTOS_SRC_DIR)/queue.c                    \
            $(RTOS_SRC_DIR)/stream_buffer.c            \
            $(RTOS_SRC_DIR)/tasks.c                    \
            $(RTOS_SRC_DIR)/timers.c                   \
            $(RTOS_SRC_DIR)/portable/MemMang/heap_4.c  \
            $(RTOS_SRC_DIR)/portable/GCC/RISC-V/port.c

# Required for Codasip CLIC
RTOS_CRT0 = $(RTOS_ASM_DIR)/portASM.S

# ----[ VARIABLES ]----

RTOS_OBJS     = $(RTOS_SRCS:.c=.o)
RTOS_OBJS    := $(subst $(RTOS_DIR),RTOS,$(RTOS_OBJS))

RTOS_CRT_OBJS  = $(RTOS_CRT0:.S=.o)
RTOS_CRT_OBJS := $(subst $(RTOS_DIR),RTOS,$(RTOS_CRT_OBJS))

# ----[ TARGETS ]----

include $(BM_SDK_DIR)/share/app.mk

BUILD ?= build
BUILD_DIR := $(BUILD)/

$(BUILD_DIR)RTOS/%.o : $(RTOS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)RTOS/%.o : $(RTOS_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

# The following overrides the default recipe for $(APP).xexe in share/app.mk:
$(BUILD_DIR)$(APP).xexe: $(addprefix $(BUILD_DIR), $(APP_OBJS) $(LIB_OBJS) $(CRT_OBJ) $(RTOS_OBJS) $(RTOS_CRT_OBJS))
	$(LINK.c) -Wl,-T$(LDSCRIPT) -o $@ $^ $(LDLIBS)

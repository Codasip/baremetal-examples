# Baremetal CMSIS-RTOS2 FreeRTOS application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

BM_SDK_DIR ?= $(DEMO_DIR)/../..

# To build this demo you need the Codasip port of FreeRTOS repo:
#   https://github.com/Codasip/FreeRTOS-Kernel.git
#
#  This demo has been tested with FreeRTOS V11.2.0
#
# Check RTOS_DIR is set
$(if $(RTOS_DIR),,$(error missing RTOS_DIR, must point to the FreeRTOS Kernel repo))

# Also, you need the CMSIS-FreeRTOS (for RTOS2) and Codasip's CMSIS_6 repos:
#   https://github.com/Codasip/CMSIS_6.git
#   https://github.com/ARM-software/CMSIS-FreeRTOS.git
#
# Check CMSIS_6_DIR is set
$(if $(CMSIS_6_DIR),,$(error missing CMSIS_6_DIR, must point to Codasip's CMSIS_6 repo))
# Check CMSIS_FR_DIR is set
$(if $(CMSIS_FR_DIR),,$(error missing CMSIS_FR_DIR, must point to the CMSIS-FreeRTOS repo))

REQUIRES += uart gpio_io

# After configuring baremetal-examples/config.mk and adding environment variable SDK_PREFIX
# cd to this directory and make

# CMSIS_6 RTOS2 API Wrapper
CMSIS_6_RT2_DIR = $(CMSIS_6_DIR)/CMSIS/RTOS2

# FreeRTOS Directories
RTOS_SRC_DIR = $(RTOS_DIR)
RTOS_ASM_DIR = $(RTOS_DIR)/portable/GCC/RISC-V

# CMSIS_FreeRTOS RTOS2 API
RTOS2_FR_DIR = $(CMSIS_FR_DIR)/CMSIS/RTOS2/FreeRTOS

# Demo App and FreeRTOS Include Directories
CPPFLAGS += -I $(LIB_DIR)/include                      \
            -I $(LIB_DIR)/targets/platforms            \
            -I $(DEMO_DIR)/src	                       \
            -I $(DEMO_DIR)/src/CMSIS                   \
            -I $(DEMO_DIR)/src/FreeRTOS                \
            -I $(RTOS_SRC_DIR)/include                 \
            -I $(RTOS_SRC_DIR)/portable/GCC/RISC-V     \
            -I $(RTOS_SRC_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RISCV_MTIME_CLINT_CLIC_no_extensions \
            -I $(CMSIS_FR_DIR)/Demo/Common/include     \
            -I $(RTOS2_FR_DIR)/Include                 \
            -I $(RTOS2_FR_DIR)/Config                  \
            -I $(CMSIS_6_DIR)/CMSIS/Core/Include       \
            -I $(CMSIS_6_RT2_DIR)/Include              \
            -D__WEAK="__attribute__((weak))"

APP       = CMSIS-FreeRTOS-demo
SOURCES   = $(DEMO_DIR)/src/CMSIS-FreeRTOS-demo.c      \
            $(DEMO_DIR)/src/MsgQueueDemo.c             \
            $(DEMO_DIR)/src/portCMSIS2FreeRTOS.c       \
            $(DEMO_DIR)/src/FreeRTOS/vector.S

RTOS_SRCS = $(RTOS_SRC_DIR)/event_groups.c             \
            $(RTOS_SRC_DIR)/list.c                     \
            $(RTOS_SRC_DIR)/queue.c                    \
            $(RTOS_SRC_DIR)/stream_buffer.c            \
            $(RTOS_SRC_DIR)/tasks.c                    \
            $(RTOS_SRC_DIR)/timers.c                   \
            $(RTOS_SRC_DIR)/portable/MemMang/heap_4.c  \
            $(RTOS_SRC_DIR)/portable/GCC/RISC-V/port.c

RTOS2_FR_SRCS = $(RTOS2_FR_DIR)/Source/clib_os.c       \
                $(RTOS2_FR_DIR)/Source/cmsis_os2.c     \
                $(RTOS2_FR_DIR)/Source/freertos_evr.c

#               $(RTOS2_FR_DIR)/Source/handlers.c currently not required


# Required for Codasip CLIC
RTOS_CRT0 = $(RTOS_ASM_DIR)/portASM.S

# ----[ VARIABLES ]----

RTOS_OBJS      = $(RTOS_SRCS:.c=.o)
RTOS_OBJS     := $(subst $(RTOS_DIR),RTOS,$(RTOS_OBJS))

RTOS2_FR_OBJS  = $(RTOS2_FR_SRCS:.c=.o)
RTOS2_FR_OBJS := $(subst $(RTOS2_FR_DIR),RTOS,$(RTOS2_FR_OBJS))

RTOS_CRT_OBJS  = $(RTOS_CRT0:.S=.o)
RTOS_CRT_OBJS := $(subst $(RTOS_DIR),RTOS,$(RTOS_CRT_OBJS))

# ----[ TARGETS ]----

include $(BM_SDK_DIR)/share/app.mk

BUILD ?= build
BUILD_DIR := $(BUILD)/

$(BUILD_DIR)RTOS/%.o : $(RTOS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)RTOS/%.o : $(RTOS2_FR_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)RTOS/%.o : $(RTOS_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

# The following overrides the default recipe for $(APP).xexe in share/app.mk:
$(BUILD_DIR)$(APP).xexe: $(addprefix $(BUILD_DIR), $(APP_OBJS) $(LIB_OBJS) $(CRT_OBJ) $(RTOS_OBJS) $(RTOS2_FR_OBJS) $(RTOS_CRT_OBJS))
	$(LINK.c) -Wl,-T$(LDSCRIPT) -o $@ $^ $(LDLIBS)

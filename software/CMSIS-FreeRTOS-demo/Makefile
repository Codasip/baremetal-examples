# Baremetal CMSIS-RTOS2 FreeRTOS application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

# To build this demo you need the CMSIS_6 and CMSIS-FreeRTOS repos, clone them in to
# a new directory (CMSIS) above the baremetal-examples root dir:
#	git clone https://github.com/ARM-software/CMSIS_6.git
#	git clone https://github.com/ARM-software/CMSIS-FreeRTOS.git
#
#   This demo has been tested with FreeRTOS V11.1.0
#
# Change the following to point to the relevant directories:
CMSIS_DIR    ?= ../CMSIS						# Where the above repos have been cloned
CMSIS_6_DIR  ?= $(CMSIS_DIR)/CMSIS_6
CMSIS_FR_DIR ?= $(CMSIS_DIR)/CMSIS-FreeRTOS

REQUIRES += uart gpio_io

# After configuring baremetal-examples/config.mk and adding environment variable SDK_PREFIX
# cd to this directory and make

# CMSIS_6 RTOS2
CMSIS_6_RT2_DIR = $(CMSIS_6_DIR)/CMSIS/RTOS2

# FreeRTOS Directories
RTOS_SRC_DIR = $(CMSIS_FR_DIR)/Source
RTOS_ASM_DIR = $(CMSIS_FR_DIR)/Source/portable/GCC/RISC-V
RTOS2_FR_DIR = $(CMSIS_FR_DIR)/CMSIS/RTOS2/FreeRTOS

# Demo App and FreeRTOS Include Directories
CPPFLAGS += -I $(LIB_DIR)/include -I $(LIB_DIR)/targets/platforms -I $(DEMO_DIR)/src	\
			-I $(DEMO_DIR)/src/CMSIS -I $(DEMO_DIR)/src/FreeRTOS \
			-I $(RTOS_SRC_DIR)/include -I $(RTOS_SRC_DIR)/portable/GCC/RISC-V \
			-I $(RTOS_SRC_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions \
			-I $(CMSIS_FR_DIR)/Demo/Common/include \
			-I $(RTOS2_FR_DIR)/Include \
			-I $(RTOS2_FR_DIR)/Config \
			-I $(CMSIS_6_DIR)/CMSIS/Core/Include \
			-I $(CMSIS_6_RT2_DIR)/Include \
			-D__WEAK="__attribute__((weak))"

APP       =	CMSIS-FreeRTOS-demo
SOURCES   =	$(DEMO_DIR)/src/CMSIS-FreeRTOS-demo.c \
			$(DEMO_DIR)/src/MsgQueueDemo.c \
			$(DEMO_DIR)/src/portCMSIS2FreeRTOS.c \
			$(DEMO_DIR)/src/FreeRTOS/vector.S \
			$(DEMO_DIR)/src/FreeRTOS/portASM_CLIC.S	# Local portASM.S required for CLIC

RTOS_SRCS =	$(RTOS_SRC_DIR)/event_groups.c \
			$(RTOS_SRC_DIR)/list.c \
			$(RTOS_SRC_DIR)/queue.c \
			$(RTOS_SRC_DIR)/stream_buffer.c \
			$(RTOS_SRC_DIR)/tasks.c \
			$(RTOS_SRC_DIR)/timers.c \
			$(RTOS_SRC_DIR)/portable/MemMang/heap_4.c \
			$(RTOS_SRC_DIR)/portable/GCC/RISC-V/port.c \
			$(RTOS2_FR_DIR)/Source/clib_os.c \
			$(RTOS2_FR_DIR)/Source/cmsis_os2.c \
			$(RTOS2_FR_DIR)/Source/freertos_evr.c

#			$(RTOS2_FR_DIR)/Source/handlers.c currently not required


# Required for CLIC
# RTOS_CRT0 = $(RTOS_ASM_DIR)/portASM.S

# ----[ VARIABLES ]----

RTOS_OBJS     = $(RTOS_SRCS:.c=.o)
RTOS_OBJS    := $(subst $(CMSIS_FR_DIR),RTOS,$(RTOS_OBJS))

RTOS_CRT_OBJS  = $(RTOS_CRT0:.S=.o)
RTOS_CRT_OBJS := $(subst $(CMSIS_FR_DIR),RTOS,$(RTOS_CRT_OBJS))

# ----[ TARGETS ]----

include $(DEMO_DIR)/../../share/app.mk

BUILD ?= build
BUILD_DIR := $(BUILD)/

$(BUILD_DIR)RTOS/%.o : $(CMSIS_FR_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)RTOS/%.o : $(CMSIS_FR_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

# The following overrides the default recipe for $(APP).xexe in share/app.mk:
$(BUILD_DIR)$(APP).xexe: $(addprefix $(BUILD_DIR), $(APP_OBJS) $(LIB_OBJS) $(CRT_OBJ) $(RTOS_OBJS) $(RTOS_CRT_OBJS))
	$(LINK.c) -Wl,-T$(LDSCRIPT) -o $@ $^ $(LDLIBS)
	$(OBJCOPY) -O binary $@ $(@:.xexe=.bin)

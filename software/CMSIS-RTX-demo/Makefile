# Baremetal CMSIS-RTX application Makefile
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

BM_SDK_DIR ?= $(DEMO_DIR)/../..

# To build this demo you need the Codasip's CMSIS-RTX and CMSIS_6 repos:
#   https://github.com/Codasip/CMSIS_6.git
#   https://github.com/Codasip/CMSIS-RTX.git
#
# This demo has been tested with RTX V5.9.0

# Check CMSIS_6_DIR is set
$(if $(CMSIS_6_DIR),,$(error missing CMSIS_6_DIR, must point to the CMSIS-6 repo))
# Check CMSIS_RTX_DIR is set
$(if $(CMSIS_RTX_DIR),,$(error missing CMSIS_RTX_DIR, must point to the CMSIS-RTX repo))

REQUIRES += clic uart gpio_io

# After configuring baremetal-examples/config.mk and adding environment variable SDK_PREFIX
# cd to this directory and make

RTX_SRC_DIR = $(CMSIS_RTX_DIR)/Source

# Demo App and FreeRTOS Include Directories
CPPFLAGS += -I $(LIB_DIR)/include                   \
            -I $(LIB_DIR)/targets/platforms         \
            -I $(DEMO_DIR)/src                      \
            -I $(DEMO_DIR)/src/CMSIS                \
            -I $(CMSIS_RTX_DIR)/Include             \
            -I $(RTX_SRC_DIR)                       \
            -I $(CMSIS_6_DIR)/CMSIS/RTOS2/Include   \
            -I $(CMSIS_6_DIR)/CMSIS/Core/Include    \
            -D__WEAK="__attribute__((weak))"

ASFLAGS  += -I$(PLATFORM_DIR)

APP       = CMSIS-RTX-demo
SOURCES   = $(DEMO_DIR)/src/CMSIS-RTX-demo.c        \
            $(DEMO_DIR)/src/MsgQueueDemo.c          \
            $(DEMO_DIR)/src/rtx_uart.c

CM6_SRCS  = $(CMSIS_6_DIR)/CMSIS/Core/Source/codasip_clic.c       \
            $(CMSIS_6_DIR)/CMSIS/Core/Source/core_rv32_clic.c     \
            $(CMSIS_6_DIR)/CMSIS/RTOS2/Source/os_tick_rv32_clic.c

RTOS_SRCS = $(RTX_SRC_DIR)/rtx_delay.c             \
            $(RTX_SRC_DIR)/rtx_evr.c               \
            $(RTX_SRC_DIR)/rtx_lib.c               \
            $(RTX_SRC_DIR)/rtx_memory.c            \
            $(RTX_SRC_DIR)/rtx_msgqueue.c          \
            $(RTX_SRC_DIR)/rtx_semaphore.c         \
            $(RTX_SRC_DIR)/rtx_evflags.c           \
            $(RTX_SRC_DIR)/rtx_kernel.c            \
            $(RTX_SRC_DIR)/rtx_mempool.c           \
            $(RTX_SRC_DIR)/rtx_mutex.c             \
            $(RTX_SRC_DIR)/rtx_system.c            \
            $(RTX_SRC_DIR)/rtx_thread.c            \
            $(RTX_SRC_DIR)/rtx_timer.c             \
            $(RTX_SRC_DIR)/rtx_thread.c            \
            $(CMSIS_RTX_DIR)/Config/RTX_Config.c


# Required for Codasip CLIC
RTOS_CRT0 += $(RTX_SRC_DIR)/GCC/irq_rv32_clic.S

# ----[ VARIABLES ]----

CM6_OBJS       = $(CM6_SRCS:.c=.o)
CM6_OBJS      := $(subst $(CMSIS_6_DIR),RTOS,$(CM6_OBJS))

RTOS_OBJS      = $(RTOS_SRCS:.c=.o)
RTOS_OBJS     := $(subst $(CMSIS_RTX_DIR),RTOS,$(RTOS_OBJS))

RTOS_CRT_OBJS  = $(RTOS_CRT0:.S=.o)
RTOS_CRT_OBJS := $(subst $(CMSIS_RTX_DIR),RTOS,$(RTOS_CRT_OBJS))

# ----[ TARGETS ]----

include $(BM_SDK_DIR)/share/app.mk

CFLAGS  += -Werror

BUILD ?= build
BUILD_DIR := $(BUILD)/

$(BUILD_DIR)RTOS/%.o : $(CMSIS_RTX_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)RTOS/%.o : $(CMSIS_6_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)RTOS/%.o : $(CMSIS_RTX_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

# The following overrides the default recipe for $(APP).xexe in share/app.mk (removed  $(CRT_OBJ) ):
$(BUILD_DIR)$(APP).xexe: $(addprefix $(BUILD_DIR), $(APP_OBJS) $(LIB_OBJS) $(RTOS_OBJS) $(CM6_OBJS) $(RTOS_CRT_OBJS))
	$(LINK.c) -Wl,-T$(LDSCRIPT) -o $@ $^ $(LDLIBS)

# ----[ INCLUDES ]----
DEMO_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
BM_SDK_DIR ?= $(DEMO_DIR)/../..

include $(BM_SDK_DIR)/share/common.mk

# ----[ VARIABLES ]----

APP     = fsbl
SOURCES = $(DEMO_DIR)/src/main.c \
          $(DEMO_DIR)/src/loader.c \
          $(DEMO_DIR)/src/parser.c

CRT_SOURCES = $(DEMO_DIR)/src/init_ram.S

ifeq ($(CONFIG_ENVIRONMENT),FPGA_UART)
BM_SOURCES  := $(filter-out $(LIB_DIR)/syscalls/sys_uart.c, $(BM_SOURCES))
SOURCES  += $(DEMO_DIR)/src/sys_hw.c \
            $(LIB_DIR)/fatfs/diskio.c \
            $(LIB_DIR)/fatfs/ff.c \
            $(LIB_DIR)/fatfs/ffunicode.c \
            $(LIB_DIR)/fatfs/mmc.c
endif

CPPFLAGS += -I $(DEMO_DIR)/src

APP_OBJS    = $(SOURCES:.c=.o)
LIB_OBJS    = $(BM_SOURCES:.c=.o)
CRT_OBJ     = $(BM_CRT0:.S=.o)
CRT_ROM_OBJ = $(CRT_SOURCES:.S=.o)

APP_OBJS    := $(subst $(DEMO_DIR),.,$(APP_OBJS))
LIB_OBJS    := $(subst $(TOP_DIR),.,$(LIB_OBJS))
CRT_OBJ     := $(subst $(TOP_DIR),.,$(CRT_OBJ))
CRT_ROM_OBJ := $(subst $(DEMO_DIR),.,$(CRT_ROM_OBJ))

CFLAGS  += -std=gnu11 -pedantic -Wall -Wextra
CFLAGS  += -g3 -Wno-unused-command-line-argument
CFLAGS  += -Os -ffunction-sections -fdata-sections -flto
ASFLAGS += -g3 -Wno-unused-command-line-argument
LDFLAGS += -nostartfiles
LDFLAGS += -flto -Os -Wl,--gc-sections
LDFLAGS += -Wl,--defsym=_HEAP_SIZE=0x2000 -Wl,--defsym=_STACK_SIZE=0x1000

FSBL_LDSCRIPT ?= $(LD_TARGET)$(XLEN)_fsbl.ld
TSBL_LDSCRIPT ?= $(LD_TARGET)$(XLEN)_tsbl.ld

# ----[ REQUIREMENTS ]----

REQUIRES += gpio_io
REQUIRES += sdcard

ifeq ($(filter-out $(PROVIDES),id-registers),id-registers)
DEFINES += ID_REGISTERS
endif

MISSING_REQUIRES = $(filter-out $(PROVIDES),$(REQUIRES))

$(if $(MISSING_REQUIRES), \
	$(foreach req, $(MISSING_REQUIRES), \
		$(warning Missing requirement '$(req)') \
	) \
	$(error Application requires features not provided by this configuration.) \
)

# ----[ TARGETS ]----

ifeq ($(CONFIG_TSBL),Y)
	TSBL_XEXE = $(BUILD_DIR)tsbl_ram.xexe
	TSBL_BIN  = $(BUILD_DIR)tsbl_ram.bin
	TSBL_LST  = $(BUILD_DIR)tsbl_ram.lst
endif

BUILD ?= build
BUILD_DIR := $(BUILD)/

$(BUILD_DIR)%.o : $(DEMO_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)%.o : $(DEMO_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)./lib%.o : $(LIB_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)./lib%.o : $(LIB_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

.PHONY: all
all: $(BUILD_DIR)$(APP).xexe $(BUILD_DIR)$(APP)_rom.xexe $(TSBL_XEXE)

$(BUILD_DIR)$(APP).xexe: $(addprefix $(BUILD_DIR), $(APP_OBJS) $(LIB_OBJS) $(CRT_OBJ))
	$(LINK.c) -Wl,-T$(LDSCRIPT) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)$(APP)_rom.xexe: $(addprefix $(BUILD_DIR), $(APP_OBJS) $(LIB_OBJS) $(CRT_OBJ) $(CRT_ROM_OBJ))
	$(LINK.c) -Wl,-L$(DEMO_DIR)/linker -Wl,-T$(FSBL_LDSCRIPT) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)tsbl_ram.xexe: $(addprefix $(BUILD_DIR), $(APP_OBJS) $(LIB_OBJS) $(CRT_OBJ))
	$(LINK.c) -Wl,-L$(DEMO_DIR)/linker -Wl,-T$(TSBL_LDSCRIPT) -o $@ $^ $(LDLIBS)

.PHONY: lst
lst: $(BUILD_DIR)$(APP).lst $(BUILD_DIR)$(APP)_rom.lst $(TSBL_LST)

%.lst: %.xexe
	$(OBJDUMP) -fhp $^ >> $@
	$(OBJDUMP) -t $^ | grep -E "^[0-9a-fA-F]{8}" | sort >> $@
	$(OBJDUMP) -dSx $^ >> $@

.PHONY: bin
bin: $(BUILD_DIR)$(APP).bin $(BUILD_DIR)$(APP)_rom.bin $(TSBL_BIN)

%.bin: %.xexe
	$(OBJCOPY) -O binary $^ $@

.PHONY: clean
clean:
	rm -rf \
      $(addprefix $(BUILD_DIR), \
        $(APP_OBJS) \
        $(LIB_OBJS) \
        $(CRT_OBJ) \
        $(CRT_ROM_OBJ) \
        $(APP).xexe \
        $(APP).bin \
        $(APP).lst \
        $(APP)_rom.xexe \
        $(APP)_rom.bin \
        $(APP)_rom.lst \
        $(TSBL_XEXE) \
        $(TSBL_BIN) \
        $(TSBL_LST) \
      ) \
      $(BUILD_DIR)

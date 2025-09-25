# ----[ INCLUDES ]----

THIS_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

include $(THIS_DIR)/common.mk

# the application's makefile must set DEMO_DIR
ifndef DEMO_DIR
$(error DEMO_DIR is not set)
endif

# ----[ VARIABLES ]----

APP_OBJS      = $(SOURCES:.c=.o)
APP_OBJS     := $(APP_OBJS:.S=.o)
APP_OBJS     := $(foreach obj, $(APP_OBJS), \
                    $(if $(filter $(DEMO_DIR)/%, $(obj)), \
                         $(patsubst $(DEMO_DIR)/%, ./app/%, $(obj)), \
                         $(if $(filter $(LIB_DIR)/%, $(obj)), \
                             $(patsubst $(LIB_DIR)/%, ./lib/%, $(obj)), \
                             ./ext/$(obj) \
                         ) \
                     ) \
                 )

LIB_OBJS      = $(BM_SOURCES:.c=.o)
LIB_OBJS     := $(LIB_OBJS:.S=.o)
LIB_OBJS     := $(patsubst $(LIB_DIR)/%, ./lib/%, $(LIB_OBJS))

CRT_OBJ       = $(BM_CRT0:.S=.o)
CRT_OBJ      := $(patsubst $(LIB_DIR)/%, ./lib/%, $(CRT_OBJ))

CFLAGS  += -std=gnu11 -pedantic -Wall -Wextra -Os
CFLAGS  += -g3 -Wno-unused-command-line-argument -ffunction-sections -fdata-sections
ASFLAGS += -g3 -Wno-unused-command-line-argument -ffunction-sections -fdata-sections
LDFLAGS += -nostartfiles -Wl,--gc-sections

# ----[ REQUIREMENTS ]----

MISSING_REQUIRES = $(filter-out $(PROVIDES),$(REQUIRES))

$(if $(MISSING_REQUIRES), \
	$(foreach req, $(MISSING_REQUIRES), \
		$(warning Missing requirement '$(req)') \
	) \
	$(error Application requires features not provided by this configuration.) \
)

# ----[ TARGETS ]----

BUILD ?= build
BUILD_DIR := $(BUILD)/

$(BUILD_DIR)./app/%.o : $(DEMO_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)./lib/%.o : $(LIB_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)./ext/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)./app/%.o : $(DEMO_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)./lib/%.o : $(LIB_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)./ext/%.o : %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

.PHONY: all
all: $(BUILD_DIR)$(APP).xexe

$(BUILD_DIR)$(APP).xexe: $(addprefix $(BUILD_DIR), $(APP_OBJS) $(LIB_OBJS) $(CRT_OBJ))
	$(LINK.c) -Wl,-T$(LDSCRIPT) -o $@ $^ $(LDLIBS)

.PHONY: lst
lst: $(BUILD_DIR)$(APP).lst

%.lst: %.xexe
	$(OBJDUMP) -afhp $^ > $@
	$(OBJDUMP) -t $^ | grep -E "^[0-9a-fA-F]{8}" | sort >> $@
	$(OBJDUMP) -drS $^ >> $@

.PHONY: bin
bin: $(BUILD_DIR)$(APP).bin

%.bin: %.xexe
	$(OBJCOPY) -O binary $^ $@

.PHONY: run
run: $(BUILD_DIR)$(APP).xexe
	$(if $(SIM),,$(error SIM variable must be set to run simulator))
	$(SIM) -r $^

.PHONY: clean
clean:
	rm -rf \
      $(addprefix $(BUILD_DIR), \
        $(APP_OBJS) \
        $(LIB_OBJS) \
        $(CRT_OBJ) \
        $(APP).bin \
        $(APP).lst \
        $(APP).xexe \
      ) \
      $(BUILD_DIR)

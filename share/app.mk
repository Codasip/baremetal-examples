# ----[ INCLUDES ]----

THIS_DIR := $(subst /app.mk,,$(lastword $(MAKEFILE_LIST)))

include $(THIS_DIR)/common.mk

# ----[ VARIABLES ]----

APP_OBJS      = $(SOURCES:.c=.o)
APP_OBJS     := $(APP_OBJS:.S=.o)
LIB_OBJS      = $(BM_SOURCES:.c=.o)
CRT_OBJ       = $(BM_CRT0:.S=.o)

APP_OBJS     := $(subst $(DEMO_DIR),.,$(APP_OBJS))
LIB_OBJS     := $(subst $(TOP_DIR),.,$(LIB_OBJS))
CRT_OBJ      := $(subst $(TOP_DIR),.,$(CRT_OBJ))

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

$(BUILD_DIR)%.o : $(DEMO_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)./lib%.o : $(LIB_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)%.o : $(DEMO_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD_DIR)./lib%.o : $(LIB_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

.PHONY: all
all: $(BUILD_DIR)$(APP).xexe

$(BUILD_DIR)$(APP).xexe: $(addprefix $(BUILD_DIR), $(APP_OBJS) $(LIB_OBJS) $(CRT_OBJ))
	$(LINK.c) -Wl,-T$(LDSCRIPT) -o $@ $^ $(LDLIBS)

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
	rm -rf $(addprefix $(BUILD_DIR), $(APP_OBJS) $(LIB_OBJS) $(CRT_OBJ) $(APP).bin $(APP).xexe) $(BUILD_DIR)

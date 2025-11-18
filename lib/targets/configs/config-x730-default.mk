
CONFIG_PROCESSOR            = X730

# ----[ CORE CONFIGURATION ]----

CONFIG_NUM_HARTS            ?= 1
CONFIG_HAS_PMP              = N
CONFIG_HAS_TCMS             = N

# ----[ PLATFORM CONFIGURATION ]----

CONFIG_PLATFORM             ?= Hobgoblin$(HOB_PLATFORM_VERSION_STR)
CONFIG_CORE_FREQ            ?= 50
CONFIG_PLIC                 = Y
CONFIG_SECURITY             = N
CONFIG_TSBL                 = Y


CONFIG_PROCESSOR            = A730

# ----[ CORE CONFIGURATION ]----

CONFIG_NUM_HARTS            ?= 1
CONFIG_HAS_PMP              = Y
CONFIG_PMP_NUM_REGIONS      = 16
CONFIG_HAS_TCMS             = N

# ----[ PLATFORM CONFIGURATION ]----

CONFIG_PLATFORM             ?= Hobgoblin$(HOB_PLATFORM_VERSION_STR)
CONFIG_CORE_FREQ            ?= 50
CONFIG_PLIC                 = Y
CONFIG_SECURITY             = N
CONFIG_TSBL                 = Y

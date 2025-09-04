
CONFIG_PROCESSOR            = L730

# ----[ CORE CONFIGURATION ]----

CONFIG_NUM_HARTS            ?= 1
CONFIG_HAS_FPU              = Y
CONFIG_HAS_PMP              = Y
CONFIG_PMP_NUM_REGIONS      = 16
CONFIG_HAS_TCMS             = Y
CONFIG_HAS_CACHES           = Y
CONFIG_HAS_HPM              = Y
CONFIG_CLIC_NUM_INTERRUPTS  = 128
CONFIG_CLIC_INTCTLBITS      = 8

# ----[ PLATFORM CONFIGURATION ]----

CONFIG_PLATFORM             ?= Hobbs$(HOB_PLATFORM_VERSION_STR)
CONFIG_CORE_FREQ            ?= 50
CONFIG_PLIC                 = N
CONFIG_SECURITY             = N
CONFIG_TSBL                 = Y


CONFIG_PROCESSOR            = V730

# ----[ CORE CONFIGURATION ]----

CONFIG_NUM_HARTS            ?= 1
CONFIG_HAS_FPU              = Y
CONFIG_HAS_FPU_DP           = N
CONFIG_HAS_PMP              = N
CONFIG_HAS_TCMS             = Y
CONFIG_HAS_CACHES           = Y
CONFIG_HAS_HPM              = Y
CONFIG_CLIC_NUM_INTERRUPTS  = 128
CONFIG_CLIC_INTCTLBITS      = 8

# ----[ PLATFORM CONFIGURATION ]----

CONFIG_PLATFORM             ?= Hobbs_v2
CONFIG_CORE_FREQ            ?= 50
CONFIG_PLIC                 = N
CONFIG_SECURITY             = N
CONFIG_TSBL                 = Y

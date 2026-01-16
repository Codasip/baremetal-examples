
CONFIG_PROCESSOR            = L110

# ----[ CORE CONFIGURATION ]----

CONFIG_CLIC_NUM_INTERRUPTS  = 16
CONFIG_CLIC_INTCTLBITS      = 3

# ----[ PLATFORM CONFIGURATION ]----

CONFIG_PLATFORM             ?= Hobbs_v2
CONFIG_CORE_FREQ            ?= 50
CONFIG_PLIC                 = N
CONFIG_SECURITY             = N
CONFIG_TSBL                 = Y

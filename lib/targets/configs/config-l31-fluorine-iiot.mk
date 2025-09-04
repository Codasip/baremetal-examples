
CONFIG_PROCESSOR            = L31

# ----[ CORE CONFIGURATION ]----

CONFIG_HAS_FPU              = N
CONFIG_HAS_PMP              = Y
CONFIG_PMP_NUM_REGIONS      = 8
CONFIG_HAS_TCMS             = N
CONFIG_HAS_CACHES           = N
CONFIG_HAS_HPM              = Y

# ----[ PLATFORM CONFIGURATION ]----

CONFIG_PLATFORM             ?= DoomBar
CONFIG_CORE_FREQ            ?= 25
CONFIG_PIC_NUM_INTERRUPTS   = 32

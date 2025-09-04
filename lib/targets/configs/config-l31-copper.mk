
CONFIG_PROCESSOR            = L31

# ----[ CORE CONFIGURATION ]----

CONFIG_HAS_FPU              = Y
CONFIG_HAS_TCMS             = N
CONFIG_HAS_CACHES           = Y
CONFIG_HAS_HPM              = N

# ----[ PLATFORM CONFIGURATION ]----

CONFIG_PLATFORM             ?= Inferno
CONFIG_CORE_FREQ            ?= 25
CONFIG_PIC_NUM_INTERRUPTS   = 32

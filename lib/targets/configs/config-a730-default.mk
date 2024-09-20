# ----[ PLATFORM CONFIGURATION ]----

CONFIG_PROCESSOR            = A730
CONFIG_PLATFORM             = Hobgoblin

#CONFIG_ENVIRONMENT         ?= FPGA_SEMIHOSTING
#CONFIG_ENVIRONMENT         ?= FPGA_UART

# ----[ CORE CONFIGURATION ]----

CONFIG_CORE_FREQ            ?= 50
CONFIG_NUM_HARTS            ?= 1
CONFIG_HAS_PMP              = N

# ----[ PLATFORM CONFIGURATION ]----

CONFIG_PLIC                 = Y
CONFIG_SECURITY             = N

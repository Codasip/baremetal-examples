# Codasip L730 Core

The Codasip L730 core is a dual-issue, in-order core and a part of the Codasip RISC-V Processors 700 processor family.

Supported configuration options:
- `CONFIG_CORE_FREQ` - core clock frequency
- `CONFIG_HAS_FPU` - configurations with Floating Point Unit
- `CONFIG_HAS_FPU_DP` - configurations with Double Precision Floating Point Unit
- `CONFIG_HAS_PMP` - configurations with Physical Memory Protection
- `CONFIG_NUM_HARTS` - number of hardware threads in the cluster
- `CONFIG_CLIC_NUM_INTERRUPTS` - number of interrupt ports
- `CONFIG_CLIC_INTCTLBITS` - CLICINTCTLBITS parameter for CLIC

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#define TARGET_CORE_NAME "L730"

#define TARGET_CLK_FREQ  CONFIG_CORE_FREQ
#define TARGET_NUM_HARTS CONFIG_NUM_HARTS

#define TARGET_HAS_CLIC
#define TARGET_CLIC_NUM_INPUTS CONFIG_CLIC_NUM_INTERRUPTS
#define TARGET_CLIC_INTCTLBITS CONFIG_CLIC_INTCTLBITS

#define TARGET_EXT_U
#define TARGET_EXT_S

#undef CLIC_TARGET_EXT_S /* The CLIC Supervisor interrupts are not conencted in the FPGA Platforms
                          * and the remaining interrupts are not shifted up by another 2,
                          * so this code adjustment for supervisor mode has been removed for now */

#define TARGET_HAS_HPM
#define TARGET_HAS_CUSTOM_CSR
#define TARGET_HAS_CACHE

#ifdef CONFIG_HAS_PMP
    #define TARGET_HAS_PMP
    #define TARGET_PMP_NUM_REGIONS 16
#endif

#ifdef CONFIG_HAS_CACHES
    #define TARGET_HAS_CACHE
#endif

#ifdef CONFIG_HAS_TCMS
    #define TARGET_HAS_TCM
#endif

#define TARGET_LINUX_SUPPORT

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#define TARGET_CORE_NAME "L31"

#define TARGET_CLK_FREQ  CONFIG_CORE_FREQ
#define TARGET_NUM_HARTS 1

#define TARGET_EXT_U

#define TARGET_HAS_PIC
#define TARGET_PIC_NUM_INTERRUPTS CONFIG_PIC_NUM_INTERRUPTS
#define TARGET_HAS_CUSTOM_CSR
#define TARGET_HAS_HPM

#ifdef CONFIG_HAS_PMP
    #define TARGET_HAS_PMP
    #define TARGET_PMP_NUM_REGIONS CONFIG_PMP_NUM_REGIONS
#endif

#ifdef CONFIG_HAS_CACHES
    #define TARGET_HAS_CACHE
#endif

#ifdef CONFIG_HAS_TCMS
    #define TARGET_HAS_TCM
#endif

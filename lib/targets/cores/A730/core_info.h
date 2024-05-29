/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#define TARGET_CORE_NAME "A730"

#define TARGET_CLK_FREQ  CONFIG_CORE_FREQ
#define TARGET_NUM_HARTS CONFIG_NUM_HARTS

#define TARGET_EXT_U
#define TARGET_EXT_S

#define TARGET_HAS_HPM
#define TARGET_HAS_CUSTOM_CSR
#define TARGET_HAS_CACHE

#ifdef CONFIG_HAS_PMP
    #define TARGET_HAS_PMP
    #define TARGET_PMP_NUM_REGIONS 16
#endif

#ifdef CONFIG_HAS_TCMS
    #define TARGET_HAS_TCM
#endif

#define TARGET_LINUX_SUPPORT

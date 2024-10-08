/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#define TARGET_CORE_NAME "L110"

#define TARGET_CLK_FREQ  CONFIG_CORE_FREQ
#define TARGET_NUM_HARTS 1

#define TARGET_HAS_CLIC
#define TARGET_CLIC_NUM_INPUTS CONFIG_CLIC_NUM_INTERRUPTS
#define TARGET_CLIC_INTCTLBITS CONFIG_CLIC_INTCTLBITS

#define TARGET_HAS_CUSTOM_CSR

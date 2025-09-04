/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "core_info.h"

.section .text

.global _core_init
_core_init:

#ifdef TARGET_CBI_ENABLE
#   if TARGET_CBI_ENABLE
    // Enable CBI
    li t0, CONFIG_CBI_MASK
    csrw mcbigs, t0
#   endif
#endif

    la t0, __jvt_base$
#if CODASIP_RV32
    csrw jvt, t0    # Table jump base vector and control register
#else
    csrw 0x17, t0   # jvt CSR Register
#endif
    ret

    .weak __jvt_base$

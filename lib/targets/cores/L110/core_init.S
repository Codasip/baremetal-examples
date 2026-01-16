/* Copyright 2024-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "core_info.h"

.section .text

.global _core_init
_core_init:

    /*
     * Initialize integer registers (prevents X's in RTL simulation)
     *
     * Skip the `x0` (`zero`) register, it always contains 0.
     * Skip the `x1` (`ra`) register, as it holds the return address.
     */
    li x2, 0
    li x3, 0
    li x4, 0
    li x5, 0
    li x6, 0
    li x7, 0
    li x8, 0
    li x9, 0
    li x10, 0
    li x11, 0
    li x12, 0
    li x13, 0
    li x14, 0
    li x15, 0
#ifndef __riscv_32e
    li x16, 0
    li x17, 0
    li x18, 0
    li x19, 0
    li x20, 0
    li x21, 0
    li x22, 0
    li x23, 0
    li x24, 0
    li x25, 0
    li x26, 0
    li x27, 0
    li x28, 0
    li x29, 0
    li x30, 0
    li x31, 0
#endif // __riscv_32e

    /* Initialize non-reset CSRs (prevents X's in RTL simulation) */
    csrsi mcountinhibit, 0x5
    csrw mcycleh, x0
    csrw mcycle, x0
    csrw minstreth, x0
    csrw minstret, x0
    csrci mcountinhibit, 0x5

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

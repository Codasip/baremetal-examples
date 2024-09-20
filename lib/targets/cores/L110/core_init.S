/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

.section .text

.global _core_init
_core_init:
    la t0, __jvt_base$
#if CODASIP_RV32
    csrw jvt, t0    # Table jump base vector and control register
#else
    csrw 0x17, t0   # jvt CSR Register
#endif
    ret

    .weak __jvt_base$

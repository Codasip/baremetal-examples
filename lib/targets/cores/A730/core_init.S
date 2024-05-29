/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * A730 core initialization routine
 *
 * This file contains assembly method for initializing the
 * A730 core to a known state, namely setting the non-reset
 * registers.
 */

#define MSTATUS_FS 0x00002000

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

    /* Enable FPU (set the FS bit in mstatus */
    li t0, MSTATUS_FS
    csrs mstatus, t0

    /* Initialize float registers (prevents X's in RTL simulation) */
    fscsr zero
    fmv.w.x f0, zero
    fmv.w.x f1, zero
    fmv.w.x f2, zero
    fmv.w.x f3, zero
    fmv.w.x f4, zero
    fmv.w.x f5, zero
    fmv.w.x f6, zero
    fmv.w.x f7, zero
    fmv.w.x f8, zero
    fmv.w.x f9, zero
    fmv.w.x f10, zero
    fmv.w.x f11, zero
    fmv.w.x f12, zero
    fmv.w.x f13, zero
    fmv.w.x f14, zero
    fmv.w.x f15, zero
    fmv.w.x f16, zero
    fmv.w.x f17, zero
    fmv.w.x f18, zero
    fmv.w.x f19, zero
    fmv.w.x f20, zero
    fmv.w.x f21, zero
    fmv.w.x f22, zero
    fmv.w.x f23, zero
    fmv.w.x f24, zero
    fmv.w.x f25, zero
    fmv.w.x f26, zero
    fmv.w.x f27, zero
    fmv.w.x f28, zero
    fmv.w.x f29, zero
    fmv.w.x f30, zero
    fmv.w.x f31, zero

    /* Initialize non-reset CSRs (prevents X's in RTL simulation) */
    csrw mcycle, zero
    csrw minstret, zero
    csrw mhpmcounter3, zero
    csrw mhpmcounter4, zero
    csrw mhpmcounter5, zero
    csrw mhpmcounter6, zero

    ret

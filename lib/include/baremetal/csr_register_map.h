/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_CSR_REGISTER_MAP_H
#define BAREMETAL_CSR_REGISTER_MAP_H

#include "baremetal/common.h"

// Helper macros for PMP config registers
#define TARGET_HAS_PMP_CFG_REG1 ((TARGET_PMP_NUM_REGIONS > 4) && (RISCV_XLEN == 32))
#define TARGET_HAS_PMP_CFG_REG2 (TARGET_PMP_NUM_REGIONS > 8)
#define TARGET_HAS_PMP_CFG_REG3 ((TARGET_PMP_NUM_REGIONS > 12) && (RISCV_XLEN == 32))

/** \brief List of standard control and status registers available on the target */
#ifdef TARGET_EXT_N
    #define BM_CSR_USTATUS 0x000
#endif /* TARGET_EXT_N */

#ifdef __riscv_flen
    #define BM_CSR_FFLAGS 0x001
    #define BM_CSR_FRM    0x002
    #define BM_CSR_FCSR   0x003
#endif /* __riscv_flen */

#ifdef TARGET_EXT_N
    #define BM_CSR_UIE      0x004
    #define BM_CSR_UTVEC    0x005
    #define BM_CSR_USCRATCH 0x040
    #define BM_CSR_UEPC     0x041
    #define BM_CSR_UCAUSE   0x042
    #define BM_CSR_UTVAL    0x043
    #define BM_CSR_UIP      0x044
#endif /* TARGET_EXT_N */

#define BM_CSR_SEED 0x015

#ifdef TARGET_EXT_S
    #define BM_CSR_SSTATUS 0x100

    #ifdef TARGET_EXT_N
        #define BM_CSR_SEDELEG 0x102
        #define BM_CSR_SIDELEG 0x103
    #endif /* TARGET_EXT_N */

    #define BM_CSR_SIE        0x104
    #define BM_CSR_STVEC      0x105
    #define BM_CSR_SCOUNTEREN 0x106
    #define BM_CSR_SSCRATCH   0x140
    #define BM_CSR_SEPC       0x141
    #define BM_CSR_SCAUSE     0x142
    #define BM_CSR_STVAL      0x143
    #define BM_CSR_SIP        0x144
    #define BM_CSR_SATP       0x180

    #ifdef TARGET_HAS_CLIC
        #define BM_CSR_STVT         0x107
        #define BM_CSR_SNXTI        0x145
        #define BM_CSR_SINTTHRESH   0x147
        #define BM_CSR_SSCRATCHCSW  0x148
        #define BM_CSR_SSCRATCHCSWL 0x149
        #define BM_CSR_SINTSTATUS   0xdb1
    #endif /* TARGET_HAS_CLIC */

    #define BM_CSR_SENVCFG 0x10a
#endif /* TARGET_EXT_S */

#define BM_CSR_MSTATUS 0x300
#define BM_CSR_MISA    0x301

#if defined(TARGET_EXT_S) || defined(TARGET_EXT_N)
    #define BM_CSR_MEDELEG 0x302
    #define BM_CSR_MIDELEG 0x303
#endif /* #if defined(TARGET_EXT_S) || defined(TARGET_EXT_N) */

#define BM_CSR_MIE   0x304
#define BM_CSR_MTVEC 0x305

#ifdef TARGET_HAS_CLIC
    #define BM_CSR_MTVT         0x307
    #define BM_CSR_MNXTI        0x345
    #define BM_CSR_MINTTHRESH   0x347
    #define BM_CSR_MSCRATCHCSW  0x348
    #define BM_CSR_MSCRATCHCSWL 0x349
    #define BM_CSR_MINTSTATUS   0xfb1
#endif /* TARGET_HAS_CLIC */

#define BM_CSR_MENVCFG 0x30a
#if RISCV_XLEN == 32
    #define BM_CSR_MENVCFGH 0x31a
#endif /* #if RISCV_XLEN == 32 */

#define BM_CSR_MCOUNTEREN    0x306
#define BM_CSR_MCOUNTINHIBIT 0x320

#ifdef TARGET_HAS_HPM
    #define BM_CSR_MHPMEVENT3  0x323
    #define BM_CSR_MHPMEVENT4  0x324
    #define BM_CSR_MHPMEVENT5  0x325
    #define BM_CSR_MHPMEVENT6  0x326
    #define BM_CSR_MHPMEVENT7  0x327
    #define BM_CSR_MHPMEVENT8  0x328
    #define BM_CSR_MHPMEVENT9  0x329
    #define BM_CSR_MHPMEVENT10 0x32a
    #define BM_CSR_MHPMEVENT11 0x32b
    #define BM_CSR_MHPMEVENT12 0x32c
    #define BM_CSR_MHPMEVENT13 0x32d
    #define BM_CSR_MHPMEVENT14 0x32e
    #define BM_CSR_MHPMEVENT15 0x32f
    #define BM_CSR_MHPMEVENT16 0x330
    #define BM_CSR_MHPMEVENT17 0x331
    #define BM_CSR_MHPMEVENT18 0x332
    #define BM_CSR_MHPMEVENT19 0x333
    #define BM_CSR_MHPMEVENT20 0x334
    #define BM_CSR_MHPMEVENT21 0x335
    #define BM_CSR_MHPMEVENT22 0x336
    #define BM_CSR_MHPMEVENT23 0x337
    #define BM_CSR_MHPMEVENT24 0x338
    #define BM_CSR_MHPMEVENT25 0x339
    #define BM_CSR_MHPMEVENT26 0x33a
    #define BM_CSR_MHPMEVENT27 0x33b
    #define BM_CSR_MHPMEVENT28 0x33c
    #define BM_CSR_MHPMEVENT29 0x33d
    #define BM_CSR_MHPMEVENT30 0x33e
    #define BM_CSR_MHPMEVENT31 0x33f
#endif /* TARGET_HAS_HPM */

#define BM_CSR_MSCRATCH 0x340
#define BM_CSR_MEPC     0x341
#define BM_CSR_MCAUSE   0x342
#define BM_CSR_MTVAL    0x343
#define BM_CSR_MIP      0x344

#ifdef TARGET_HAS_PMP
    #define BM_CSR_PMPCFG0 0x3a0

    #if TARGET_HAS_PMP_CFG_REG1
        #define BM_CSR_PMPCFG1 0x3a1
    #endif /* TARGET_HAS_PMP_CFG_REG1 */

    #if TARGET_HAS_PMP_CFG_REG2
        #define BM_CSR_PMPCFG2 0x3a2
    #endif // TARGET_HAS_PMP_CFG_REG2

    #if TARGET_HAS_PMP_CFG_REG3
        #define BM_CSR_PMPCFG3 0x3a3
    #endif /* TARGET_HAS_PMP_CFG_REG3 */

    #define BM_CSR_PMPADDR0 0x3b0
    #define BM_CSR_PMPADDR1 0x3b1
    #define BM_CSR_PMPADDR2 0x3b2
    #define BM_CSR_PMPADDR3 0x3b3

    #if TARGET_PMP_NUM_REGIONS > 4
        #define BM_CSR_PMPADDR4 0x3b4
        #define BM_CSR_PMPADDR5 0x3b5
        #define BM_CSR_PMPADDR6 0x3b6
        #define BM_CSR_PMPADDR7 0x3b7
    #endif /* #if TARGET_PMP_NUM_REGIONS > 4 */

    #if TARGET_PMP_NUM_REGIONS > 8
        #define BM_CSR_PMPADDR8  0x3b8
        #define BM_CSR_PMPADDR9  0x3b9
        #define BM_CSR_PMPADDR10 0x3ba
        #define BM_CSR_PMPADDR11 0x3bb
    #endif /* #if TARGET_PMP_NUM_REGIONS > 8 */

    #if TARGET_PMP_NUM_REGIONS > 12
        #define BM_CSR_PMPADDR12 0x3bc
        #define BM_CSR_PMPADDR13 0x3bd
        #define BM_CSR_PMPADDR14 0x3be
        #define BM_CSR_PMPADDR15 0x3bf
    #endif /* #if TARGET_PMP_NUM_REGIONS > 12 */

#endif /* TARGET_HAS_PMP */

#define BM_CSR_MSECCFG 0x747

#if RISCV_XLEN == 32
    #define BM_CSR_MSECCFGH 0x757
#endif /* #if RISCV_XLEN == 32 */

#define BM_CSR_TSELECT  0x7a0
#define BM_CSR_TDATA1   0x7a1
#define BM_CSR_TDATA2   0x7a2
#define BM_CSR_TDATA3   0x7a3
#define BM_CSR_TINFO    0x7a4
#define BM_CSR_TCONTROL 0x7a5
#define BM_CSR_MCONTEXT 0x7a8

#ifdef TARGET_HAS_PIC
    #define BM_CSR_MPICFLAG 0x7c1
    #define BM_CSR_MPICMASK 0x7c2

    #if TARGET_PIC_NUM_INTERRUPTS > 32
        #define BM_CSR_MPICFLAG1 0x7c4
        #define BM_CSR_MPICMASK1 0x7c5
    #endif /* #if TARGET_PIC_NUM_INTERRUPTS > 32 */

    #if TARGET_PIC_NUM_INTERRUPTS > 64
        #define BM_CSR_MPICFLAG2 0x7c7
        #define BM_CSR_MPICMASK2 0x7c8
    #endif /* #if TARGET_PIC_NUM_INTERRUPTS > 64 */

    #if TARGET_PIC_NUM_INTERRUPTS > 96
        #define BM_CSR_MPICFLAG3 0x7ca
        #define BM_CSR_MPICMASK3 0x7cb
    #endif /* #if TARGET_PIC_NUM_INTERRUPTS > 96 */

#endif /* TARGET_HAS_PIC */

#define BM_CSR_MCYCLE   0xb00
#define BM_CSR_MINSTRET 0xb02

#ifdef TARGET_HAS_HPM
    #define BM_CSR_MHPMCOUNTER3  0xb03
    #define BM_CSR_MHPMCOUNTER4  0xb04
    #define BM_CSR_MHPMCOUNTER5  0xb05
    #define BM_CSR_MHPMCOUNTER6  0xb06
    #define BM_CSR_MHPMCOUNTER7  0xb07
    #define BM_CSR_MHPMCOUNTER8  0xb08
    #define BM_CSR_MHPMCOUNTER9  0xb09
    #define BM_CSR_MHPMCOUNTER10 0xb0a
    #define BM_CSR_MHPMCOUNTER11 0xb0b
    #define BM_CSR_MHPMCOUNTER12 0xb0c
    #define BM_CSR_MHPMCOUNTER13 0xb0d
    #define BM_CSR_MHPMCOUNTER14 0xb0e
    #define BM_CSR_MHPMCOUNTER15 0xb0f
    #define BM_CSR_MHPMCOUNTER16 0xb10
    #define BM_CSR_MHPMCOUNTER17 0xb11
    #define BM_CSR_MHPMCOUNTER18 0xb12
    #define BM_CSR_MHPMCOUNTER19 0xb13
    #define BM_CSR_MHPMCOUNTER20 0xb14
    #define BM_CSR_MHPMCOUNTER21 0xb15
    #define BM_CSR_MHPMCOUNTER22 0xb16
    #define BM_CSR_MHPMCOUNTER23 0xb17
    #define BM_CSR_MHPMCOUNTER24 0xb18
    #define BM_CSR_MHPMCOUNTER25 0xb19
    #define BM_CSR_MHPMCOUNTER26 0xb1a
    #define BM_CSR_MHPMCOUNTER27 0xb1b
    #define BM_CSR_MHPMCOUNTER28 0xb1c
    #define BM_CSR_MHPMCOUNTER29 0xb1d
    #define BM_CSR_MHPMCOUNTER30 0xb1e
    #define BM_CSR_MHPMCOUNTER31 0xb1f
#endif /* TARGET_HAS_HPM */

#if RISCV_XLEN == 32
    #define BM_CSR_MCYCLEH   0xb80
    #define BM_CSR_MINSTRETH 0xb82

    #ifdef TARGET_HAS_HPM
        #define BM_CSR_MHPMCOUNTER3H  0xb83
        #define BM_CSR_MHPMCOUNTER4H  0xb84
        #define BM_CSR_MHPMCOUNTER5H  0xb85
        #define BM_CSR_MHPMCOUNTER6H  0xb86
        #define BM_CSR_MHPMCOUNTER7H  0xb87
        #define BM_CSR_MHPMCOUNTER8H  0xb88
        #define BM_CSR_MHPMCOUNTER9H  0xb89
        #define BM_CSR_MHPMCOUNTER10H 0xb8a
        #define BM_CSR_MHPMCOUNTER11H 0xb8b
        #define BM_CSR_MHPMCOUNTER12H 0xb8c
        #define BM_CSR_MHPMCOUNTER13H 0xb8d
        #define BM_CSR_MHPMCOUNTER14H 0xb8e
        #define BM_CSR_MHPMCOUNTER15H 0xb8f
        #define BM_CSR_MHPMCOUNTER16H 0xb90
        #define BM_CSR_MHPMCOUNTER17H 0xb91
        #define BM_CSR_MHPMCOUNTER18H 0xb92
        #define BM_CSR_MHPMCOUNTER19H 0xb93
        #define BM_CSR_MHPMCOUNTER20H 0xb94
        #define BM_CSR_MHPMCOUNTER21H 0xb95
        #define BM_CSR_MHPMCOUNTER22H 0xb96
        #define BM_CSR_MHPMCOUNTER23H 0xb97
        #define BM_CSR_MHPMCOUNTER24H 0xb98
        #define BM_CSR_MHPMCOUNTER25H 0xb99
        #define BM_CSR_MHPMCOUNTER26H 0xb9a
        #define BM_CSR_MHPMCOUNTER27H 0xb9b
        #define BM_CSR_MHPMCOUNTER28H 0xb9c
        #define BM_CSR_MHPMCOUNTER29H 0xb9d
        #define BM_CSR_MHPMCOUNTER30H 0xb9e
        #define BM_CSR_MHPMCOUNTER31H 0xb9f
    #endif /* TARGET_HAS_HPM */

#endif /* #if RISCV_XLEN == 32 */

#define BM_CSR_CYCLE   0xc00
#define BM_CSR_TIME    0xc01
#define BM_CSR_INSTRET 0xc02

#ifdef TARGET_HAS_HPM
    #define BM_CSR_HPMCOUNTER3  0xc03
    #define BM_CSR_HPMCOUNTER4  0xc04
    #define BM_CSR_HPMCOUNTER5  0xc05
    #define BM_CSR_HPMCOUNTER6  0xc06
    #define BM_CSR_HPMCOUNTER7  0xc07
    #define BM_CSR_HPMCOUNTER8  0xc08
    #define BM_CSR_HPMCOUNTER9  0xc09
    #define BM_CSR_HPMCOUNTER10 0xc0a
    #define BM_CSR_HPMCOUNTER11 0xc0b
    #define BM_CSR_HPMCOUNTER12 0xc0c
    #define BM_CSR_HPMCOUNTER13 0xc0d
    #define BM_CSR_HPMCOUNTER14 0xc0e
    #define BM_CSR_HPMCOUNTER15 0xc0f
    #define BM_CSR_HPMCOUNTER16 0xc10
    #define BM_CSR_HPMCOUNTER17 0xc11
    #define BM_CSR_HPMCOUNTER18 0xc12
    #define BM_CSR_HPMCOUNTER19 0xc13
    #define BM_CSR_HPMCOUNTER20 0xc14
    #define BM_CSR_HPMCOUNTER21 0xc15
    #define BM_CSR_HPMCOUNTER22 0xc16
    #define BM_CSR_HPMCOUNTER23 0xc17
    #define BM_CSR_HPMCOUNTER24 0xc18
    #define BM_CSR_HPMCOUNTER25 0xc19
    #define BM_CSR_HPMCOUNTER26 0xc1a
    #define BM_CSR_HPMCOUNTER27 0xc1b
    #define BM_CSR_HPMCOUNTER28 0xc1c
    #define BM_CSR_HPMCOUNTER29 0xc1d
    #define BM_CSR_HPMCOUNTER30 0xc1e
    #define BM_CSR_HPMCOUNTER31 0xc1f
#endif /* TARGET_HAS_HPM */

#if RISCV_XLEN == 32
    #define BM_CSR_CYCLEH   0xc80
    #define BM_CSR_TIMEH    0xc81
    #define BM_CSR_INSTRETH 0xc82

    #ifdef TARGET_HAS_HPM
        #define BM_CSR_HPMCOUNTER3H  0xc83
        #define BM_CSR_HPMCOUNTER4H  0xc84
        #define BM_CSR_HPMCOUNTER5H  0xc85
        #define BM_CSR_HPMCOUNTER6H  0xc86
        #define BM_CSR_HPMCOUNTER7H  0xc87
        #define BM_CSR_HPMCOUNTER8H  0xc88
        #define BM_CSR_HPMCOUNTER9H  0xc89
        #define BM_CSR_HPMCOUNTER10H 0xc8a
        #define BM_CSR_HPMCOUNTER11H 0xc8b
        #define BM_CSR_HPMCOUNTER12H 0xc8c
        #define BM_CSR_HPMCOUNTER13H 0xc8d
        #define BM_CSR_HPMCOUNTER14H 0xc8e
        #define BM_CSR_HPMCOUNTER15H 0xc8f
        #define BM_CSR_HPMCOUNTER16H 0xc90
        #define BM_CSR_HPMCOUNTER17H 0xc91
        #define BM_CSR_HPMCOUNTER18H 0xc92
        #define BM_CSR_HPMCOUNTER19H 0xc93
        #define BM_CSR_HPMCOUNTER20H 0xc94
        #define BM_CSR_HPMCOUNTER21H 0xc95
        #define BM_CSR_HPMCOUNTER22H 0xc96
        #define BM_CSR_HPMCOUNTER23H 0xc97
        #define BM_CSR_HPMCOUNTER24H 0xc98
        #define BM_CSR_HPMCOUNTER25H 0xc99
        #define BM_CSR_HPMCOUNTER26H 0xc9a
        #define BM_CSR_HPMCOUNTER27H 0xc9b
        #define BM_CSR_HPMCOUNTER28H 0xc9c
        #define BM_CSR_HPMCOUNTER29H 0xc9d
        #define BM_CSR_HPMCOUNTER30H 0xc9e
        #define BM_CSR_HPMCOUNTER31H 0xc9f
    #endif /* TARGET_HAS_HPM */

#endif /* #if RISCV_XLEN == 32 */

#define BM_CSR_MVENDORID 0xf11
#define BM_CSR_MARCHID   0xf12
#define BM_CSR_MIMPID    0xf13
#define BM_CSR_MHARTID   0xf14

#endif /* BAREMETAL_CSR_REGISTER_MAP_H */

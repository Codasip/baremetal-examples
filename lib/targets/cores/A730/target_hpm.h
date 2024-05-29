/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TARGET_HPM_H
#define TARGET_HPM_H

/** \brief Enumeration of HPM events */
typedef enum {
    BM_HPM_EXCEPTIONS        = 0x001, // RISC-V Exception taken
    BM_HPM_INTERRUPTS        = 0x003, // Interrupt taken
    BM_HPM_LOADS_COMMITTED   = 0x004, // Integer/FP Load committed
    BM_HPM_STORES_COMMITTED  = 0x005, // Integer/FP Store committed
    BM_HPM_ATOMICS_COMMITTED = 0x006, // Integer/FP Atomic committed
    BM_HPM_ZERO_INSTR_AVAIL = 0x800, // Cycles with zero instructions available from the Instruction Queue
    BM_HPM_ONE_INSTR_AVAIL = 0x801, // Cycles with one instruction available from the Instruction Queue
    BM_HPM_TWO_INSTR_AVAIL = 0x802, // Cycles with two instructions available from the Instruction Queue
    BM_HPM_ZERO_INSTR_ISSUED = 0x803, // Cycles with zero instructions issued
    BM_HPM_ONE_INSTR_ISSUED  = 0x804, // Cycles with one instruction issued
    BM_HPM_TWO_INSTR_ISSUED  = 0x805, // Cycles with two instructions issued
    BM_HPM_BRANCH_MISSES     = 0x900, // Conditional branch misprediction
    BM_HPM_JUMP_MISSES       = 0x901, // Jump target misprediction
    BM_HPM_LSU_STALLS        = 0x905, // LSU stall cycle
} bm_hpm_event_t;

#endif /* TARGET_HPM_H */

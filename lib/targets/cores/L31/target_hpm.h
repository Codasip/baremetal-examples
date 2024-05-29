/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TARGET_HPM_H
#define TARGET_HPM_H

/** \brief Enumeration of HPM events */
typedef enum {
    BM_HPM_PIPE_FLUSHES,  // Total count the internal pipe was flushed
    BM_HPM_CSR_FLUSHES,   // Total count of flushes caused by CSR writes
    BM_HPM_EXCEPTIONS,    // Total count of exceptions
    BM_HPM_INTERRUPTS,    // Total count of interrupts
    BM_HPM_BRANCH_MISSES, // Total count of branch misses
    BM_HPM_LOAD_STALLS,   // Time the core spent handling load-use stalls
    BM_HPM_DIV_STALLS,    // Time the core spent handling the division unit stalls.
    BM_HPM_FETCH_STALLS,  // Time the core spent handling the instruction fetch unit stalls.
    BM_HPM_LDST_STALLS,   // Time the core spent handling the load/store unit stalls.
    BM_HPM_FLOAT_STALLS,  // Time the core spent handling the floating point unit stalls.
    BM_HPM_EVENTS_COUNT,
} bm_hpm_event_t;

#endif /* TARGET_HPM_H */

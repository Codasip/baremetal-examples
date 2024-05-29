/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_MEM_BARRIER_H
#define BAREMETAL_MEM_BARRIER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Execute fence instruction
 */
static inline void bm_exec_fence(void)
{
    __asm__ volatile("fence rw, rw");
}

/**
 * \brief Execute fence.i instruction
 */
static inline void bm_exec_fence_i(void)
{
    __asm__ volatile("fence.i");
}

/**
 * \brief Execute sfence.vma instruction
 */
static inline void bm_exec_sfence_vma(void)
{
    __asm__ volatile("sfence.vma");
}

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_MEM_BARRIER_H */

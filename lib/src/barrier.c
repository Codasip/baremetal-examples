/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/barrier.h"

#include "baremetal/common.h"
#include "baremetal/mem_barrier.h"
#include "baremetal/mp.h"

#include <stdint.h>

#if TARGET_NUM_HARTS == 1
void bm_barrier_init(bm_barrier_t *barrier UNUSED) {}
void bm_barrier_wait(bm_barrier_t *barrier UNUSED) {}
#elif !defined(__riscv_atomic)
    #error "Systems with multiple harts and no atomic instructions are not supported"
#else

void bm_barrier_init(bm_barrier_t *barrier)
{
    barrier->waiting = 0;
    barrier->done    = 0;

    // Ensure updated data is visible from all harts
    bm_exec_fence();
}

void bm_barrier_wait(bm_barrier_t *barrier)
{
    uint32_t tmp = 1;
    __asm__ volatile("amoadd.w x0, %0, (%1)\n" : : "r"(tmp), "r"(&barrier->waiting));

    // Wait until all harts are in this loop
    while (barrier->waiting != TARGET_NUM_HARTS)
        ;

    __asm__ volatile("amoadd.w x0, %0, (%1)\n" : : "r"(tmp), "r"(&barrier->done));

    if (bm_get_hartid() == 0)
    {
        // Wait until all harts are past the main loop
        while (barrier->done != TARGET_NUM_HARTS)
            ;

        // Clean the barrier
        bm_barrier_init(barrier);
    }
    else
    {
        // Wait until the barrier is cleaned
        while (barrier->done != 0)
            ;
    }
}
#endif

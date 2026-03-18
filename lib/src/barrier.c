/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/barrier.h"

#include "baremetal/atomic.h"
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
    unsigned hartid;

    barrier->waiting = 0;
    for (hartid = 0; hartid < TARGET_NUM_HARTS; hartid++)
    {
        barrier->counter[hartid] = 0;
    }
    barrier->done = 0;

    // Ensure updated data is visible from all harts
    bm_exec_fence();
}

// This is a sense-counting barrier, it prevents a subtle race condition that can happen
// with a “reset-and-go” barrier.
//
// Subtle race condition: without a per-hart counter (or sense variable),
// a hart can enter the next barrier while others are still spinning in
// the previous one, causing deadlock. This barrier avoids that by tagging
// each hart's participation (sense-counting).
void bm_barrier_wait(bm_barrier_t *barrier)
{
    unsigned hartid = bm_get_hartid();
    uint32_t old;

    // Increment this hart's counter
    barrier->counter[hartid]++;

    // Atomically increment the number of harts waiting in this barrier
    bm_amoadd_w(old, 1, &barrier->waiting);

    if (old == TARGET_NUM_HARTS - 1)
    {
        // This is the last hart to reach this barrier, so release it
        barrier->waiting = 0;
        barrier->done++;

        // Ensure updated data is visible from all harts
        bm_exec_fence();
    }
    else
    {
        // Wait until this barrier is released
        while (barrier->counter[hartid] != barrier->done)
        {}
    }
}
#endif

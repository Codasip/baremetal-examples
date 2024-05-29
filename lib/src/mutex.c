/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/mutex.h"

#include "baremetal/common.h"
#include "baremetal/mem_barrier.h"

#include <stdint.h>

#if TARGET_NUM_HARTS == 1
void bm_mutex_init(bm_mutex_t *mutex UNUSED) {}
int  bm_mutex_trylock(bm_mutex_t *mutex UNUSED)
{
    return 0;
}
void bm_mutex_lock(bm_mutex_t *mutex UNUSED) {}
void bm_mutex_unlock(bm_mutex_t *mutex UNUSED) {}
#elif !defined(__riscv_atomic)
    #error "Systems with multiple harts and no atomic instructions are not supported"
#else

void bm_mutex_init(bm_mutex_t *mutex)
{
    *mutex = 0;

    // Ensure updated data is visible from all harts
    bm_exec_fence();
}

int bm_mutex_trylock(bm_mutex_t *mutex)
{
    bm_mutex_t swp = 1;
    __asm__ volatile("amoswap.w %0, %1, (%2)\n" : "=r"(swp) : "r"(swp), "r"(mutex));
    return (int)swp;
}

void bm_mutex_lock(bm_mutex_t *mutex)
{
    while (bm_mutex_trylock(mutex))
        ;
}

void bm_mutex_unlock(bm_mutex_t *mutex)
{
    __asm__ volatile("amoswap.w x0, x0, (%0)\n" ::"r"(mutex));
}
#endif

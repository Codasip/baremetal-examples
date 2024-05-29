/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/barrier.h>
#include <baremetal/clint.h>
#include <baremetal/common.h>
#include <baremetal/interrupt.h>
#include <baremetal/mp.h>
#include <baremetal/mutex.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdio.h>
#include <stdlib.h>

static bm_clint_t  *clint;
static bm_mutex_t   mutex;
static bm_barrier_t barrier;

/**
 * \brief Function for handling MSIP (software) interrupt
 */
void msip_handler(void)
{
    unsigned hart_id = bm_get_hartid();

    // Clear pending interrupt
    bm_clint_clear_ipi(clint, hart_id);

    printf("Hart%u: MSIP interrupt received!\n", hart_id);
}

/**
 * \brief Function to be caled from each hart
 */
void hart_job(bm_hart_func_arg_t arg UNUSED)
{
    // Configure interrupt handling
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);
    bm_interrupt_set_handler(BM_INTERRUPT_MSIP, msip_handler);
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MSIP);

    // Make sure all harts are ready before continuing
    bm_barrier_wait(&barrier);

    // The rest of the function is in a critical section
    bm_mutex_lock(&mutex);

    unsigned hart_id   = bm_get_hartid();
    unsigned next_hart = (hart_id + 1) % TARGET_NUM_HARTS;

    printf("Hart%u: sending IPI to hart%u\n", hart_id, next_hart);

    // Send IPI to the next available hart
    bm_clint_send_ipi(clint, next_hart);

    bm_mutex_unlock(&mutex);
}

int main(void)
{
    puts("Welcome to the CLINT-IPI demo!\n");

    clint = (bm_clint_t *)target_peripheral_get(BM_PERIPHERAL_CLINT);

    bm_mutex_init(&mutex);
    bm_barrier_init(&barrier);

    // Run on all harts
    bm_hart_execute_all((bm_hart_func_ptr_t)hart_job);

    puts("Bye.");
    return EXIT_SUCCESS;
}

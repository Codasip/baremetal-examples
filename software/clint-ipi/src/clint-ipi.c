/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/atomic.h>
#include <baremetal/barrier.h>
#include <baremetal/clint.h>
#include <baremetal/common.h>
#include <baremetal/interrupt.h>
#include <baremetal/mp.h>
#include <baremetal/mutex.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static bm_clint_t       *clint;
static bm_mutex_t        mutex = 0;
static bm_barrier_t      barrier;
static volatile uint32_t done_ctr = 0;

/**
 * \brief Function for handling MSIP (software) interrupt
 */
void msip_handler(bm_register_file_t *stacked_regs)
{
    (void)stacked_regs; // unused

    unsigned hart_id = bm_get_hartid();

    // Clear pending interrupt
    bm_clint_clear_ipi(clint, hart_id);

    // Output in a critical mutex section
    bm_mutex_lock(&mutex);

    printf("Hart%u: MSIP interrupt received!\n", hart_id);

    bm_mutex_unlock(&mutex);

#if TARGET_NUM_HARTS == 1
    // Flag that this has finished
    done_ctr++;
#elif !defined(__riscv_atomic)
    #error "Systems with multiple harts and no atomic instructions are not supported"
#else
    // Atomically increment the number of harts that have finished
    uint32_t rd; /* Result not used */

    bm_amoadd_w(rd, 1, &done_ctr);
#endif
}

/**
 * \brief Function to be called from each hart
 */
void hart_job(bm_hart_func_arg_t arg UNUSED)
{
    // Configure interrupt handling
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);
    bm_interrupt_set_handler(BM_INTERRUPT_MSIP, msip_handler);
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MSIP);

    bm_interrupt_disable(BM_PRIV_MODE_MACHINE);

    // Make sure all harts are ready before continuing
    bm_barrier_wait(&barrier);

    unsigned hart_id   = bm_get_hartid();
    unsigned next_hart = (hart_id + 1) % TARGET_NUM_HARTS;

    // Output in a critical mutex section
    bm_mutex_lock(&mutex);

    printf("Hart%u: sending IPI to hart%u\n", hart_id, next_hart);

    bm_mutex_unlock(&mutex);

    // Make sure all harts are ready before continuing
    bm_barrier_wait(&barrier);

    // Enable the interrupt after the above barrier to prevent possible deadlock with the mutex in the msip_handler() ISR
    bm_interrupt_enable(BM_PRIV_MODE_MACHINE);

    // Send IPI to the next available hart
    bm_clint_send_ipi(clint, next_hart);
}

int main(void)
{
    // Note: Only hart 0 is running this main function, because crt0.S calls bm_park_hart() for all other harts
    puts("Welcome to the CLINT-IPI demo!\n");

    clint = (bm_clint_t *)target_peripheral_get(BM_PERIPHERAL_CLINT);

    bm_mutex_init(&mutex);
    bm_barrier_init(&barrier);

    // Run on all harts
    bm_hart_execute_all((bm_hart_func_ptr_t)hart_job);

    // Wait until all harts are done
    while (done_ctr != TARGET_NUM_HARTS)
    {}

    // Output in a critical section
    puts("Bye.");

    return EXIT_SUCCESS;
}

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/clint.h>
#include <baremetal/common.h>
#include <baremetal/counter.h>
#include <baremetal/interrupt.h>
#include <baremetal/mp.h>
#include <baremetal/platform.h>
#include <baremetal/priv.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bm_clint_t       *clint;
static unsigned          clint_tics_in_hundred_ms;
static volatile unsigned mtip_counter;

/**
 * Interrupt handler for the MTIP (timer) interrupt
 */
void mtip_handler(void)
{
    mtip_counter++;

    bm_clint_rearm_timer(clint, bm_get_hartid(), clint_tics_in_hundred_ms);
}

/**
 * /brief Simple function measuring number of instructions and cycles spent waiting for 10 timer interrupts
 *
 * \param use_wfi Flag to configure whether to use busy-wait or wfi instructions
 */
void measure_intr(bool use_wfi)
{
    // Restart the timer
    bm_clint_arm_timer(clint, bm_get_hartid(), clint_tics_in_hundred_ms);

    mtip_counter = 0;
    bm_counter_clear(BM_COUNTER_INSTRET);
    bm_counter_clear(BM_COUNTER_CYCLE);

    // Wait until 10 interrupts are triggered
    while (mtip_counter < 10)
    {
        if (use_wfi)
        {
            bm_wfi();
        }
    }

    // Read the counters
    xlen_t mcycle_val   = bm_counter_read(BM_COUNTER_CYCLE);
    xlen_t minstret_val = bm_counter_read(BM_COUNTER_INSTRET);

    printf("Test took " BM_FMT_XLEN " cycles, with " BM_FMT_XLEN " instructions executed.\n\n",
           mcycle_val,
           minstret_val);
}

int main(void)
{
    puts("Welcome to the WFI demo!\n");

    clint                    = (bm_clint_t *)target_peripheral_get(BM_PERIPHERAL_CLINT);
    clint_tics_in_hundred_ms = bm_clint_ms_to_ticks(clint, 100);

    // Setup interrupt handler and CLINT to generate interrupts
    bm_clint_arm_timer(clint, bm_get_hartid(), clint_tics_in_hundred_ms);
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);
    bm_interrupt_set_handler(BM_INTERRUPT_MTIP, mtip_handler);
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MTIP);

    puts("Running sequence without WFIs:");
    measure_intr(false);

    puts("Running sequence with WFIs:");
    measure_intr(true);

    puts("Bye.");
    return EXIT_SUCCESS;
}

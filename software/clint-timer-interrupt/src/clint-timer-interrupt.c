/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/clint.h>
#include <baremetal/interrupt.h>
#include <baremetal/interrupt_low.h>
#include <baremetal/mp.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdio.h>
#include <stdlib.h>

static bm_clint_t *clint;
static unsigned    clint_tics_in_second;

/**
 * \brief Function for handling MTIP (timer) interrupt
 */
void mtip_handler(void)
{
    // Configure CLINT to issue an interrupt after 1000 milliseconds
    bm_clint_rearm_timer(clint, bm_get_hartid(), clint_tics_in_second);

    puts("One second elapsed.");
}

int main(void)
{
    puts("Welcome to the CLINT-timer-interrupt demo!\n");

    // Configure CLINT to issue an interrupt after 1000 milliseconds
    clint                = (bm_clint_t *)target_peripheral_get(BM_PERIPHERAL_CLINT);
    clint_tics_in_second = bm_clint_ms_to_ticks(clint, 1000);
    bm_clint_arm_timer(clint, bm_get_hartid(), clint_tics_in_second);

    // Initialize interrupt handling
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);
    bm_interrupt_set_handler(BM_INTERRUPT_MTIP, mtip_handler);
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MTIP);

    // Let the core run for a while to generate a few interrupts
    bm_delay_ms(3000);

    // Stop the generation of interrupts
    bm_interrupt_disable(BM_PRIV_MODE_MACHINE);

    puts("Bye.");
    return EXIT_SUCCESS;
}

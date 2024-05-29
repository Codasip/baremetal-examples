/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/counter.h>
#include <baremetal/time.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * \brief Print value of cycle and instruction counters
 */
static inline void print_counters(void)
{
    printf("  - Cycle counter       : 0x%016" PRIx64 "\n", bm_counter_read(BM_COUNTER_CYCLE));
    printf("  - Instruction counter : 0x%016" PRIx64 "\n\n", bm_counter_read(BM_COUNTER_INSTRET));
}

int main(void)
{
    puts("Welcome to the counter demo!\n");

    // Let the core run for a while to let counters increment
    bm_delay_ms(100);

    puts("Counter values at the start of the program:");
    print_counters();

    // Clear counters
    bm_counter_clear(BM_COUNTER_CYCLE);
    bm_counter_clear(BM_COUNTER_INSTRET);

    // Pause incrementing the instruction counter
    bm_counter_pause(BM_COUNTER_INSTRET);

    puts("Counter values after clearing and pausing instruction counter:");
    print_counters();

    // Let the core run for a while, instruction counter should not increment
    bm_delay_ms(100);

    puts("Counter values after a little while:");
    print_counters();

    bm_counter_resume(BM_COUNTER_INSTRET);

    // Let the core run for a while, instruction counter should increment again
    bm_delay_ms(100);

    puts("Counter values after resuming instruction counter:");
    print_counters();

    puts("Bye.");
    return EXIT_SUCCESS;
}

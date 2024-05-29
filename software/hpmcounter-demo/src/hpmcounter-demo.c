/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/counter.h>
#include <baremetal/hpm.h>
#include <baremetal/time.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_COUNT 1000

// Defaults defined in Makefile depending on target core
#ifndef TEST_HPM_EVENT
    #error "Define tested HPM event"
#endif

// Variables used in the test examples
static xlen_t a = (xlen_t)&a;
static xlen_t b = (xlen_t)&b;

/** \brief Function executing two independent load instructions */
void independent_loads()
{
    __asm__ volatile("lw t1, 0 (%0)\n"
                     "lw t2, 0 (%1)\n" ::"r"(a),
                     "r"(b)
                     : "t1", "t2");
}

/** \brief Function executing two load instructions, value obtained by the first one is used by the second */
void dependant_loads()
{
    // Note, that variable b is unused, but still present, to instruct
    // the compiler to generate similar code as in independent loads function
    __asm__ volatile("lw t1, 0 (%0)\n"
                     "lw t2, 0 (t1)\n" ::"r"(a),
                     "r"(b)
                     : "t1", "t2");
}

/** \brief Function executing several dependant load instructions with nops inbetween */
void sparse_loads()
{
    __asm__ volatile("lw t1, 0 (%0)\n"
                     "nop\n"
                     "nop\n"
                     "lw t1, 0 (t1)\n"
                     "nop\n"
                     "nop\n"
                     "lw t1, 0 (t1)\n"
                     "nop\n"
                     "nop\n"
                     "lw t1, 0 (t1)\n"
                     "nop\n"
                     "nop\n"
                     "lw t1, 0 (t1)\n"
                     "nop\n"
                     "nop\n" ::"r"(a)
                     : "t1");
}

/** \brief Function executing several dependant load instructions and nops afterwards */
void grouped_loads()
{
    __asm__ volatile("lw t1, 0 (%0)\n"
                     "lw t1, 0 (t1)\n"
                     "lw t1, 0 (t1)\n"
                     "lw t1, 0 (t1)\n"
                     "lw t1, 0 (t1)\n"
                     "nop\n"
                     "nop\n"
                     "nop\n"
                     "nop\n"
                     "nop\n"
                     "nop\n"
                     "nop\n"
                     "nop\n"
                     "nop\n"
                     "nop\n" ::"r"(a)
                     : "t1");
}

/**
 * Simple function for measurement of performance in given test sequence
 *
 * \param test Test sequence
 */
void check_counters(void (*test)())
{
    // Clear counters
    bm_counter_clear(BM_COUNTER_INSTRET);
    bm_counter_clear(BM_COUNTER_CYCLE);

    // Start HPM counter
    bm_hpmcounter_start(TEST_HPM_EVENT);

    // Repeat the test sequence several times to reduce effect of other noise on the results
    for (int i = 0; i < TEST_COUNT; ++i)
    {
        test();
    }

    // Read counters
    uint64_t hpm_ctr_val  = bm_hpmcounter_read(TEST_HPM_EVENT);
    uint64_t mcycle_val   = bm_counter_read(BM_COUNTER_CYCLE);
    uint64_t minstret_val = bm_counter_read(BM_COUNTER_INSTRET);

    // Stop the HPM counter
    bm_hpmcounter_stop(TEST_HPM_EVENT);

    // Printout counter values
    printf("  - Tested HPM counter  : 0x%016" PRIx64 "\n", hpm_ctr_val);
    printf("  - Cycle counter       : 0x%016" PRIx64 "\n", mcycle_val);
    printf("  - Instruction counter : 0x%016" PRIx64 "\n", minstret_val);
}

int main(void)
{
    puts("Welcome to the HPM counter demo!\n");

    puts("Comparing independent and dependant loads\n");
    puts("Running sequence with independent loads:");
    check_counters(independent_loads);

    puts("Running sequence with dependant loads:");
    check_counters(dependant_loads);

    puts("Comparing sparse and dense loads\n");
    puts("Running sequence with loads intertwined with nops:");
    check_counters(sparse_loads);

    puts("\nRunning sequence with loads grouped together:");
    check_counters(grouped_loads);

    puts("Bye.");
    return EXIT_SUCCESS;
}

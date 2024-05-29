/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/counter.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <target_cache.h>

#define TEST_DATA_LEN 0x4000

static volatile uint32_t test_data[TEST_DATA_LEN];

/**
 * Simple function for measurement of cache performance
 */
void test_caches()
{
    // Invalidate the caches so that previous instructions dont influence the results
    bm_dcache_invalidate_all();

    // Clear the counter
    bm_counter_clear(BM_COUNTER_CYCLE);

    // Write memory to test the cache settings
    for (unsigned i = 0; i < TEST_DATA_LEN; ++i)
    {
        test_data[i] = i;
    }
    bm_dcache_flush_all();

    // Measure elapsed time
    unsigned elapsed = bm_get_time_ms();

    printf("Test took %u milliseconds.\n\n", elapsed);
}

int main(void)
{
    puts("Welcome to the cache-write-through demo!\n");

    bm_dcache_set_writethrough(true);

    puts("Testing caches with write-through setting:");
    test_caches();

    bm_dcache_set_writethrough(false);

    puts("Testing caches with write-back setting:");
    test_caches();

    puts("Bye.");
    return EXIT_SUCCESS;
}

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <baremetal/counter.h>
#include <baremetal/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <target_cache.h>

#define TEST_DATA_LEN 0x4000

static volatile uint32_t test_data[TEST_DATA_LEN];

/**
 * \brief Generator of sequential data
 */
static unsigned sequential_access(unsigned offset)
{
    return offset;
}

/**
 * \brief Generator of random data
 */
static unsigned random_access(unsigned offset UNUSED)
{
    return rand() % TEST_DATA_LEN;
}

/**
 * \brief Measurement of cache performance in test sequences produced with given generators
 *
 * \param generator Generator of test sequences
 */
void test_caches(unsigned (*generator)(unsigned offset))
{
    // Prepare test data using given generator
    for (unsigned i = 0; i < TEST_DATA_LEN; ++i)
    {
        test_data[i] = generator(i);
    }

    // Invalidate the caches so that previous instructions dont influence the results
    bm_dcache_flush_all();
    bm_dcache_invalidate_all();

    // Clear the counters
    bm_cache_clear_counters();
    bm_counter_clear(BM_COUNTER_CYCLE);

    // Read from memory in order given by the test data
    for (unsigned i = 0; i < TEST_DATA_LEN; ++i)
    {
        (void)test_data[test_data[i]];
    }

    // Get the number of hits and misses
    xlen_t   hits    = bm_cache_get_info(BM_DCACHE_HITS);
    xlen_t   misses  = bm_cache_get_info(BM_DCACHE_MISS);
    unsigned elapsed = bm_get_time_ms();

    printf("There are %llu data cache hits and %llu misses logged, test took %u "
           "milliseconds.\n\n",
           (unsigned long long)hits,
           (unsigned long long)misses,
           elapsed);
}

int main(void)
{
    puts("Welcome to the cache counter demo!\n");

    puts("Testing sequential access:");
    test_caches(sequential_access);

    puts("Testing random access:");
    test_caches(random_access);

    puts("Bye.");
    return EXIT_SUCCESS;
}

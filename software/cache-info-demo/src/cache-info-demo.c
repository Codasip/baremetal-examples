/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <stdio.h>
#include <stdlib.h>
#include <target_cache.h>

int main(void)
{
    puts("Welcome to the cache-info demo!\n");

    printf("Cache version: %u\n\n", (unsigned)bm_cache_get_info(BM_CACHE_VERSION));

    xlen_t type = bm_cache_get_info(BM_CACHE_TYPE);

    if (type & BM_CACHE_TYPE_ICACHE)
    {
        puts("Instruction cache settings:");

        printf("Instruction cache size: %u\n", (unsigned)bm_cache_get_info(BM_ICACHE_SIZE));
        printf("Instruction cache associativity: %u-ways\n",
               (unsigned)bm_cache_get_info(BM_ICACHE_NUM_WAYS));
        printf("Instruction cache line size: %u\n\n", (unsigned)bm_cache_get_info(BM_ICACHE_LINE_SIZE));
    }

    if (type & BM_CACHE_TYPE_DCACHE)
    {
        puts("Data cache settings:");

        printf("Data cache size: %u\n", (unsigned)bm_cache_get_info(BM_DCACHE_SIZE));
        printf("Data cache associativity: %u-ways\n", (unsigned)bm_cache_get_info(BM_DCACHE_NUM_WAYS));
        printf("Data cache line size: %u\n\n", (unsigned)bm_cache_get_info(BM_DCACHE_LINE_SIZE));
    }

    puts("Bye.");
    return EXIT_SUCCESS;
}

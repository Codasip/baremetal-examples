/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/clint.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    puts("Welcome to the CLINT-timer demo!\n");

    bm_clint_t    *clint                = (bm_clint_t *)target_peripheral_get(BM_PERIPHERAL_CLINT);
    const unsigned clint_tics_in_second = bm_clint_ms_to_ticks(clint, 1000);

    // Simple setup for periodic tasks, using mtime register in CLINT
    for (unsigned i = 0; i < 3; i++)
    {
        uint64_t start = bm_clint_get_mtime(clint);

        puts("One second elapsed.");

        // Let the core run for 1000 milliseconds
        while (start + clint_tics_in_second > bm_clint_get_mtime(clint))
            ;
    }

    puts("Bye.");
    return EXIT_SUCCESS;
}

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/time.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    puts("Welcome to the timing demo!\n");

    for (unsigned i = 0; i < 3; i++)
    {
        unsigned start = bm_get_time_ms();

        // Let the core run for some time
        bm_delay_ms(1000 * i);

        unsigned end = bm_get_time_ms();

        printf("%u seconds elapsed.\n", (end - start) / 1000);
    }

    puts("Bye.");
    return EXIT_SUCCESS;
}

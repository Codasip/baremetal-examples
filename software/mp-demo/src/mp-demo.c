/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <baremetal/mp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static volatile uint32_t num_harts = 0;

/**
 * \brief Function executed from all harts, incrementing shared variable
 */
void hart_job(bm_hart_func_arg_t arg)
{
    (void)arg; // unused

    uint32_t tmp = 1;

    // Increment the variable from each hart atomically
    __asm__ volatile("amoadd.w %0, %1, (%2)\n" : "=r"(tmp) : "r"(tmp), "r"(&num_harts));
}

int main(void)
{
    puts("Welcome to the MP demo!\n");

    // start a job on each hart
    bm_hart_execute_all((bm_hart_func_ptr_t)hart_job);

    printf("Observed number of harts was %u, the expected number is %u.\n\n", num_harts, TARGET_NUM_HARTS);

    puts("Bye.");
    return EXIT_SUCCESS;
}

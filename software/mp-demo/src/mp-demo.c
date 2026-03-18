/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/atomic.h>
#include <baremetal/bm_cheri.h>
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
    bm_amoadd_w(tmp, tmp, &num_harts);
}

int main(void)
{
    puts("Welcome to the MP demo!\n");

    // start a job on each hart
    bm_hart_execute_all((bm_hart_func_ptr_t)hart_job);

    printf("Observed number of harts was %" PRIu32 ", the expected number is %u.\n\n",
           num_harts,
           (unsigned int)TARGET_NUM_HARTS);

    puts("Bye.");
    return EXIT_SUCCESS;
}

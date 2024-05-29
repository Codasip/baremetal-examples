/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <stdio.h>
#include <stdlib.h>
#include <target_tcm.h>

/** \brief Data stored in the DTCM */
BM_PLACE_IN_DTCM static unsigned dtcm_data = 0xc0ffee;

/** \brief Function executing from the ITCM */
BM_PLACE_IN_ITCM void itcm_function(void)
{
    printf("Function executing from " BM_FMT_XLEN ", found 0x%x in DTCM at " BM_FMT_XLEN ".\n",
           (xlen_t)&itcm_function,
           dtcm_data,
           (xlen_t)&dtcm_data);
}

int main(void)
{
    puts("Welcome to the TCM demo!\n");

    // Enable the TCMs
    bm_tcm_itcm_enable();
    bm_tcm_dtcm_enable();

    // Printout TCM info
    printf("Instruction TCM of size " BM_FMT_XLEN " starts at " BM_FMT_XLEN ".\n",
           bm_tcm_itcm_get_size(),
           bm_tcm_itcm_get_base_address());
    printf("Data TCM of size " BM_FMT_XLEN " starts at " BM_FMT_XLEN ".\n",
           bm_tcm_dtcm_get_size(),
           bm_tcm_dtcm_get_base_address());

    // Initialize the TCM memories with the instructions and data marked BM_PLACE_IN_*TCM
    bm_tcm_itcm_init();
    bm_tcm_dtcm_init();

    // Execute function from ITCM
    itcm_function();

    puts("Bye.");
    return EXIT_SUCCESS;
}

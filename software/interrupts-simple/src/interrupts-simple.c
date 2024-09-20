/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <baremetal/csr.h>
#include <baremetal/interrupt_low.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * \brief A custom interrupt handler
 */
void __attribute__((interrupt, aligned(64))) my_handler(void)
{
    puts("Entered interrupt handler.");

    // Move past offending instrcution to continue
    bm_csr_write(BM_CSR_MEPC, bm_csr_read(BM_CSR_MEPC) + 0x4);
}

int main(void)
{
    puts("Welcome to the interrupts-simple demo!\n");

    // Setup interrupt handler
    bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE, (xlen_t)&my_handler, BM_INTERRUPT_MODE_DIRECT);

    // Invalid instruction to induce an exception
    __asm__ volatile(".word 0x0");

    puts("Bye.");
    return EXIT_SUCCESS;
}

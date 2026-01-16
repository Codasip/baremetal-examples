/* Copyright 2023-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <baremetal/csr.h>
#include <baremetal/interrupt_low.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * \brief A custom interrupt handler
 */
void __attribute__((interrupt, aligned(TRAP_HANDLER_ALIGNMENT))) my_handler(void)
{
    puts("Entered interrupt handler.");

    // Move past offending instruction to continue
#ifdef __CHERI_PURE_CAPABILITY__
    const uint8_t *csr_val = 0;
    BM_CSR_READ_CAP(mepcc, csr_val);
    BM_CSR_WRITE_CAP(mepcc, csr_val + 0x04);

#else
    xlen_t csr_val = 0;
    BM_CSR_READ(BM_CSR_MEPC, csr_val);
    BM_CSR_WRITE(BM_CSR_MEPC, csr_val + 0x04);
#endif
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

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <baremetal/csr.h>
#include <baremetal/interrupt.h>
#include <stdio.h>

/**
 * \brief A custom handler for Ilegal Instruction exception
 */
void ilegal_instruction_handler(void)
{
    puts("Enterred custom handler, check out the what caused the exception:");

    // Should be 0x2 - ilegal instruction
    printf("  - CSR mcause : " BM_FMT_XLEN "\n", bm_csr_read(BM_CSR_MCAUSE));
    // Should be a few instructions after the address printed from main
    printf("  - CSR mepc : " BM_FMT_XLEN "\n", bm_csr_read(BM_CSR_MEPC));
    // Should be 0x0 - the binary value of the instruction
    printf("  - CSR mtval : " BM_FMT_XLEN "\n\n", bm_csr_read(BM_CSR_MTVAL));

    // Move past the offending instruction to continue
    bm_csr_write(BM_CSR_MEPC, bm_csr_read(BM_CSR_MEPC) + 0x4);
}

int main(void)
{
    puts("Welcome to the exception demo!\n");

    // Initialize interrupt and exception handling
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);

    xlen_t program_counter;
    __asm__ volatile("auipc %0, 0" : "=r"(program_counter));

    bm_exception_set_handler(BM_EXCEPTION_II, ilegal_instruction_handler);
    printf("Main function executing at " BM_FMT_XLEN "\n", program_counter);

    puts("Test custom managed exception handler:");

    // Attempt to execute an ilegal instruction
    __asm__ volatile(".word 0x0");

    bm_exception_set_handler(BM_EXCEPTION_II, NULL);
    puts("Test default managed exception handler:");

    // Attempt to execute an ilegal instruction
    __asm__ volatile(".word 0x0");
}

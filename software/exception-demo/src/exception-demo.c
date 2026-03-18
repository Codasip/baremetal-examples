/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <baremetal/csr.h>
#include <baremetal/interrupt.h>
#include <stdio.h>

/**
 * \brief A custom handler for Illegal Instruction exception
 */
void ilegal_instruction_handler(bm_register_file_t *stacked_regs)
{
    (void)stacked_regs; // unused

    puts("Entered custom handler, check out what caused the exception:");

    xlen_t csr_val = 0;

    // Should be 0x2 - illegal instruction
    BM_CSR_READ(BM_CSR_MCAUSE, csr_val);
    printf("  - CSR mcause : " BM_FMT_XLEN "\n", csr_val);

    // Should be a few instructions after the address printed from main
#ifdef __CHERI_PURE_CAPABILITY__
    const uint8_t *csr_val_c = 0;

    BM_CSR_READ_CAP(mepcc, csr_val_c);
    printf("  - CSR mepcc : %#p\n", (void *)csr_val_c);
#else
    BM_CSR_READ(BM_CSR_MEPC, csr_val);
    printf("  - CSR mepc : " BM_FMT_XLEN "\n", csr_val);
#endif

    // Should be 0x0 - the binary value of the instruction
    BM_CSR_READ(BM_CSR_MTVAL, csr_val);
    printf("  - CSR mtval : " BM_FMT_XLEN "\n\n", csr_val);

    // Move past the offending instruction to continue
#ifdef __CHERI_PURE_CAPABILITY__
    BM_CSR_READ_CAP(mepcc, csr_val_c);
    BM_CSR_WRITE_CAP(mepcc, csr_val_c + 0x04);

#else
    BM_CSR_READ(BM_CSR_MEPC, csr_val);
    BM_CSR_WRITE(BM_CSR_MEPC, csr_val + 0x04);
#endif
}

int main(void)
{
    puts("Welcome to the exception demo!\n");

    // Initialize interrupt and exception handling
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);

#ifdef __CHERI_PURE_CAPABILITY__
    const void *program_counter;
    __asm__ volatile("auipc %0, 0" : "=C"(program_counter));

    bm_exception_set_handler(BM_EXCEPTION_II, ilegal_instruction_handler);
    printf("Main function executing at %#p\n", program_counter);

#else
    xlen_t program_counter;
    __asm__ volatile("auipc %0, 0" : "=r"(program_counter));

    bm_exception_set_handler(BM_EXCEPTION_II, ilegal_instruction_handler);
    printf("Main function executing at " BM_FMT_XLEN "\n", (xlen_t)program_counter);
#endif

    puts("Test custom managed exception handler:");

    // Attempt to execute an illegal instruction
    __asm__ volatile(".word 0x0");

    bm_exception_set_handler(BM_EXCEPTION_II, NULL);
    puts("Test default managed exception handler:");

    // Attempt to execute an illegal instruction
    __asm__ volatile(".word 0x0");
}

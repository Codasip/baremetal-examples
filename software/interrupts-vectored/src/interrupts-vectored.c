/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/clint.h>
#include <baremetal/common.h>
#include <baremetal/csr.h>
#include <baremetal/interrupt_low.h>
#include <baremetal/mp.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <baremetal/verbose.h>
#include <stdio.h>
#include <stdlib.h>

static bm_clint_t *clint;
static unsigned    clint_tics_in_second;

/**
 * \brief A custom handler for unexpected interrupts
 */
void __attribute__((interrupt, aligned(16))) default_handler(void)
{
    puts("Default handler called!");
}

/**
 * \brief A custom interrupt handler for exceptions
 */
void __attribute__((interrupt, aligned(16))) exception_handler(void)
{
    puts("Exception handler running!");

    // Move past offending instrcution to continue
    bm_csr_write(BM_CSR_MEPC, bm_csr_read(BM_CSR_MEPC) + 0x4);
}

/**
 * \brief A custom interrupt handler for timer interrupts
 */
void __attribute__((interrupt, aligned(16))) interrupt_handler(void)
{
    puts("Interrupt handler running!");

    // Configure CLINT to issue an interrupt after 1000 milliseconds
    bm_clint_rearm_timer(clint, bm_get_hartid(), clint_tics_in_second);
}

void __attribute__((naked, section(".text.mtvec_table"), aligned(16))) bm_irq_mtvec_table(void)
{
    __asm__ volatile(".option push;"
                     ".option norvc;"
                     "j exception_handler;" // Exception
                     "j default_handler;"
                     "j default_handler;"
                     "j default_handler;"
                     "j default_handler;"
                     "j default_handler;"
                     "j default_handler;"
                     "j interrupt_handler;" // MTIP
                     "j default_handler;"
                     "j default_handler;"
                     "j default_handler;"
                     "j default_handler;"
                     "j default_handler;"
                     "j default_handler;"
                     "j default_handler;"
                     "j default_handler;"
                     ".option pop;");
}

int main(void)
{
    puts("Welcome to the interrupts-vectored demo!\n");

    clint                = (bm_clint_t *)target_peripheral_get(BM_PERIPHERAL_CLINT);
    clint_tics_in_second = bm_clint_ms_to_ticks(clint, 1000);

    // Configure CLINT to issue an interrupt after 1000 milliseconds
    bm_clint_arm_timer(clint, bm_get_hartid(), clint_tics_in_second);

    // Setup interrupt handler
    bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE, (xlen_t)&bm_irq_mtvec_table, BM_INTERRUPT_MODE_VECTOR);

    // Enable timer interrupts
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MTIP);
    bm_interrupt_enable(BM_PRIV_MODE_MACHINE);

    // Invalid instruction to induce an exception
    __asm__ volatile(".word 0x0");

    bm_delay_ms(2000);

    // Stop the generation of interrupts
    bm_interrupt_disable(BM_PRIV_MODE_MACHINE);

    puts("Bye.");
    return EXIT_SUCCESS;
}

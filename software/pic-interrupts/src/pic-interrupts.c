/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/csr.h>
#include <baremetal/interrupt.h>
#include <baremetal/pic.h>
#include <stdio.h>
#include <stdlib.h>

#define PIC_INTERRUPT_NUM 5

void __attribute__((interrupt, aligned(16))) my_handler(void)
{
    int pending = bm_pic_get_irq();

    if (pending == -1)
    {
        return;
    }

    puts("=== INTERRUPT HANDLER RUNNING ===\n");

    bm_pic_clear_irq(pending);
}

static void fire_interrupt(void)
{
    puts("Firing interrupt.\n");
    BM_CSR_SET(BM_CSR_MPICFLAG, 1 << PIC_INTERRUPT_NUM);
}

static void enable_interrupts(void)
{
    puts("Enabling interrupt.\n");
    bm_pic_enable_source(PIC_INTERRUPT_NUM);
}

static void disable_interrupts(void)
{
    puts("Disabling interrupt.\n");
    bm_pic_disable_source(PIC_INTERRUPT_NUM);
}

int main(void)
{
    puts("Welcome to the PIC interrupts demo!\n");

    // Setup interrupts and enable M-Mode interrupts in general.
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);

    // Don't use bare metal interrupt framework, install a custom handler.
    bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE, (xlen_t)&my_handler, BM_INTERRUPT_MODE_DIRECT);

    // Enable external interrupts.
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MEIP);

    enable_interrupts();

    fire_interrupt();

    disable_interrupts();

    fire_interrupt();

    enable_interrupts();

    puts("Bye.");
    return EXIT_SUCCESS;
}

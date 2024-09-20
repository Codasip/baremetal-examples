/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/clic.h>
#include <baremetal/csr.h>
#include <baremetal/gpio.h>
#include <baremetal/interrupt_low.h>
#include <baremetal/mem_barrier.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bm_gpio_t *gpio0;
static bm_gpio_t *gpio1;
static bm_clic_t *clic;

static void (*mtvt_table[TARGET_CLIC_NUM_INPUTS])(void) __attribute__((aligned(64))) = {0};

void __attribute__((interrupt)) gpio0_handler(void)
{
    puts("=== Handling GPIO INTERRUPT 0 ===");
    bm_gpio_clear_irq(gpio0);
}

void __attribute__((interrupt)) gpio1_handler(void)
{
    puts("=== Handling GPIO INTERRUPT 1 ===");
    bm_gpio_clear_irq(gpio1);
}

int main(void)
{
    puts("Welcome to the CLIC vectored interrupts demo!\n");

    gpio0 = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);
    gpio1 = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_SD);
    clic  = (bm_clic_t *)target_peripheral_get(BM_PERIPHERAL_CLIC);

    bm_clic_init(clic);
    bm_gpio_init_irq(gpio0);
    bm_gpio_init_irq(gpio1);

    // Setup interrupt handlers
    mtvt_table[bm_clic_get_ext_irq_id(gpio0->ext_irq_id)] = gpio0_handler;
    mtvt_table[bm_clic_get_ext_irq_id(gpio1->ext_irq_id)] = gpio1_handler;
    bm_csr_write(BM_CSR_MTVT, (xlen_t)mtvt_table);

    bm_exec_fence_i();

    // Enable external interrupts
    bm_clic_set_enable(clic, bm_clic_get_ext_irq_id(gpio0->ext_irq_id), 1);
    bm_clic_set_enable(clic, bm_clic_get_ext_irq_id(gpio1->ext_irq_id), 1);
    bm_clic_set_vectored(clic, bm_clic_get_ext_irq_id(gpio0->ext_irq_id), 1);
    bm_clic_set_vectored(clic, bm_clic_get_ext_irq_id(gpio1->ext_irq_id), 1);
    bm_interrupt_enable(BM_PRIV_MODE_MACHINE);

    // Fire interrupts
    bm_gpio_set_irq(gpio0);
    bm_gpio_set_irq(gpio1);

    puts("Bye.");
    return EXIT_SUCCESS;
}

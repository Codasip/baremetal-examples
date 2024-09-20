/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/clic.h>
#include <baremetal/gpio.h>
#include <baremetal/interrupt_low.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bm_gpio_t *gpio0;
static bm_gpio_t *gpio1;
static bm_clic_t *clic;

void __attribute__((interrupt, aligned(64))) my_handler(void)
{
    unsigned pending = bm_csr_read(BM_CSR_MCAUSE) & 0x7ff;
    if (pending == bm_clic_get_ext_irq_id(gpio0->ext_irq_id))
    {
        puts("=== Handling GPIO INTERRUPT 0 ===");
        bm_gpio_clear_irq(gpio0);
    }
    else if (pending == bm_clic_get_ext_irq_id(gpio1->ext_irq_id))
    {
        puts("=== Handling GPIO INTERRUPT 1 ===");
        bm_gpio_clear_irq(gpio1);
    }
    else
    {
        puts("Error!");
    }
}

/**
 * Fire interrupts.
 */
static inline void fire_interrupt(void)
{
    bm_gpio_set_irq(gpio0);
    bm_gpio_set_irq(gpio1);

    // Enable both at the same time
    bm_delay_ms(100);
    bm_interrupt_enable(BM_PRIV_MODE_MACHINE);
    bm_interrupt_disable(BM_PRIV_MODE_MACHINE);
}

static inline void test_priority(unsigned prio0, unsigned prio1)
{
    bm_clic_set_level(clic, bm_clic_get_ext_irq_id(gpio0->ext_irq_id), prio0);
    bm_clic_set_level(clic, bm_clic_get_ext_irq_id(gpio1->ext_irq_id), prio1);

    fire_interrupt();
}

int main(void)
{
    puts("Welcome to the CLIC priority demo!\n");

    gpio0 = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);
    gpio1 = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_SD);
    clic  = (bm_clic_t *)target_peripheral_get(BM_PERIPHERAL_CLIC);

    bm_clic_init(clic);
    bm_gpio_init_irq(gpio0);
    bm_gpio_init_irq(gpio1);

    // Setup interrupt handler
    bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE, (xlen_t)&my_handler, BM_INTERRUPT_MODE_DIRECT);

    // Enable external interrupts
    bm_clic_set_enable(clic, bm_clic_get_ext_irq_id(gpio0->ext_irq_id), 1);
    bm_clic_set_enable(clic, bm_clic_get_ext_irq_id(gpio1->ext_irq_id), 1);

    puts("Firing interrupts with higher priority for first GPIO");
    test_priority(255, 127);

    puts("Firing interrupts with higher priority for second GPIO");
    test_priority(127, 255);

    puts("Bye.");
    return EXIT_SUCCESS;
}

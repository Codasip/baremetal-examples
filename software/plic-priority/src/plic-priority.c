/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/gpio.h>
#include <baremetal/interrupt_low.h>
#include <baremetal/mp.h>
#include <baremetal/platform.h>
#include <baremetal/plic.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bm_gpio_t *gpio0;
static bm_gpio_t *gpio1;
static bm_plic_t *plic;

void __attribute__((interrupt, aligned(16))) my_handler(void)
{
    // Claim the interrupt
    int pending = bm_plic_claim(plic, bm_get_hartid());

    if (pending == -1)
    {
        return;
    }

    if (pending == (int)gpio0->ext_irq_id)
    {
        puts("=== Handling GPIO INTERRUPT 0 ===");
        bm_gpio_clear_irq(gpio0);
    }
    else if (pending == (int)gpio1->ext_irq_id)
    {
        puts("=== Handling GPIO INTERRUPT 1 ===");
        bm_gpio_clear_irq(gpio1);
    }
    else
    {
        puts("Error!");
    }

    // Finalize interrupt handling
    bm_plic_complete(plic, bm_get_hartid(), pending);
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

static inline void test_priority(unsigned prio0, unsigned prio1, unsigned threshold)
{
    bm_plic_set_threshold(plic, bm_get_hartid(), threshold);
    bm_plic_set_priority(plic, gpio0->ext_irq_id, prio0);
    bm_plic_set_priority(plic, gpio1->ext_irq_id, prio1);

    fire_interrupt();
}

int main(void)
{
    puts("Welcome to the PLIC priority demo!\n");

    gpio0 = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);
    gpio1 = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_SD);
    plic  = (bm_plic_t *)target_peripheral_get(BM_PERIPHERAL_PLIC);

    bm_gpio_init_irq(gpio0);
    bm_gpio_init_irq(gpio1);

    // Setup interrupt handler
    bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE, (xlen_t)&my_handler, BM_INTERRUPT_MODE_DIRECT);

    // Enable external interrupts
    bm_plic_set_enable(plic, bm_get_hartid(), gpio0->ext_irq_id, 1);
    bm_plic_set_enable(plic, bm_get_hartid(), gpio1->ext_irq_id, 1);
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MEIP);

    puts("\nTesting with both priorities higher than the threshold.");
    test_priority(1, 1, 0);

    puts("\nTesting with only one priority higher than the threshold.");
    test_priority(1, 2, 1);

    puts("\nTesting with both priorities higher than the threshold, but:");
    puts("First has higher priority.");
    test_priority(2, 1, 0);

    puts("Second has higher priority.");
    test_priority(1, 2, 0);

    puts("Bye.");
    return EXIT_SUCCESS;
}

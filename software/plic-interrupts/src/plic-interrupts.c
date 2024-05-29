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

static bm_gpio_t *gpio;
static bm_plic_t *plic;

void __attribute__((interrupt, aligned(16))) my_handler(void)
{
    // Claim the interrupt
    int pending = bm_plic_claim(plic, bm_get_hartid());

    if (pending == -1)
    {
        return;
    }

    puts("=== INTERRUPT HANDLER RUNNING ===\n");

    // Clear the interrupt
    bm_gpio_clear_irq(gpio);

    // Finalize interrupt handling
    bm_plic_complete(plic, bm_get_hartid(), pending);
}

static inline void fire_interrupt(void)
{
    puts("Firing interrupt.\n");
    bm_gpio_set_irq(gpio);
}

static inline void enable_interrupts(void)
{
    puts("Enabling interrupt.\n");
    bm_plic_set_enable(plic, bm_get_hartid(), gpio->ext_irq_id, true);
    bm_plic_set_priority(plic, gpio->ext_irq_id, 1);
}

static inline void disable_interrupts(void)
{
    puts("Disabling interrupt.\n");
    bm_plic_set_enable(plic, bm_get_hartid(), gpio->ext_irq_id, false);
}

int main(void)
{
    puts("Welcome to the plic-interrupts demo!\n");

    plic = (bm_plic_t *)target_peripheral_get(BM_PERIPHERAL_PLIC);
    gpio = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);
    bm_gpio_init_irq(gpio);

    // Setup interrupt handler
    bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE, (xlen_t)&my_handler, BM_INTERRUPT_MODE_DIRECT);

    // Enable external interrupts
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MEIP);
    bm_interrupt_enable(BM_PRIV_MODE_MACHINE);

    enable_interrupts();

    fire_interrupt();

    disable_interrupts();

    fire_interrupt();

    enable_interrupts();

    puts("Bye.");
    return EXIT_SUCCESS;
}

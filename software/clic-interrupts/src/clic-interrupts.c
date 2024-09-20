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

static bm_gpio_t *gpio;
static bm_clic_t *clic;

void __attribute__((interrupt, aligned(64))) my_handler(void)
{
    puts("=== INTERRUPT HANDLER RUNNING ===\n");

    // Clear the interrupt
    bm_gpio_clear_irq(gpio);
}

static inline void fire_interrupt(void)
{
    puts("Firing interrupt.\n");
    bm_gpio_set_irq(gpio);
}

static inline void enable_interrupts(void)
{
    puts("Enabling interrupt.\n");
    bm_clic_set_enable(clic, bm_clic_get_ext_irq_id(gpio->ext_irq_id), true);
}

static inline void disable_interrupts(void)
{
    puts("Disabling interrupt.\n");
    bm_clic_set_enable(clic, bm_clic_get_ext_irq_id(gpio->ext_irq_id), false);
}

int main(void)
{
    puts("Welcome to the CLIC interrupts demo!\n");

    clic = (bm_clic_t *)target_peripheral_get(BM_PERIPHERAL_CLIC);
    bm_clic_init(clic);
    gpio = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);
    bm_gpio_init_irq(gpio);

    // Setup interrupt handler
    bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE, (xlen_t)&my_handler, BM_INTERRUPT_MODE_DIRECT);
    bm_interrupt_enable(BM_PRIV_MODE_MACHINE);

    enable_interrupts();

    fire_interrupt();

    disable_interrupts();

    fire_interrupt();

    enable_interrupts();

    puts("Bye.");
    return EXIT_SUCCESS;
}

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/gpio.h>
#include <baremetal/interrupt.h>
#include <baremetal/pic.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdio.h>

bm_gpio_t *gpio;

void interrupt_handler(void)
{
    puts("GPIO interrupt generated!");

    bm_gpio_set(gpio, TARGET_GPIO_PORT_LED0);
    bm_gpio_set(gpio, TARGET_GPIO_PORT_LED1);
    bm_gpio_set(gpio, TARGET_GPIO_PORT_LED2);
    bm_gpio_set(gpio, TARGET_GPIO_PORT_LED3);
    bm_gpio_set(gpio, TARGET_GPIO_PORT_LED4);
    bm_gpio_set(gpio, TARGET_GPIO_PORT_LED5);
    bm_gpio_set(gpio, TARGET_GPIO_PORT_LED6);
    bm_gpio_set(gpio, TARGET_GPIO_PORT_LED7);

    bm_gpio_clear_irq(gpio);
}

int main(void)
{
    puts("Welcome to the GPIO interrupt demo!\n");

    gpio = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);

    bm_interrupt_init(BM_PRIV_MODE_MACHINE);
    bm_ext_irq_set_handler(gpio->ext_irq_id, interrupt_handler);
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MEIP);

    bm_gpio_init_irq(gpio);

    puts("Firing GPIO interrupt...");
    bm_gpio_set_irq(gpio);
    puts("Done.\n");

    puts("Feel free to flip switches, to trigger interrupts, and observe blinking LEDs.");

    while (1)
    {
        bm_gpio_set(gpio, TARGET_GPIO_PORT_LED0);

        bm_delay_ms(250);

        bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED0);
        bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED1);
        bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED2);
        bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED3);
        bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED4);
        bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED5);
        bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED6);
        bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED7);

        bm_delay_ms(250);
    }
}

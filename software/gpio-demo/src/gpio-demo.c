/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/gpio.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdio.h>

int main(void)
{
    puts("Welcome to the GPIO demo!\n");

    puts("Feel free to flip switches 1-3, and observe blinking LEDs.");

    bm_gpio_t *gpio = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);

    bool light = false;

    while (1)
    {
        bm_delay_ms(250);

        light = !light;

        // Blink LED 0 and also LEDs 1-3 if the corresponding switches are on
        if (light)
        {
            bm_gpio_set(gpio, TARGET_GPIO_PORT_LED0);

            if (bm_gpio_read(gpio, TARGET_GPIO_PORT_SWITCH1))
            {
                bm_gpio_set(gpio, TARGET_GPIO_PORT_LED1);
            }
            if (bm_gpio_read(gpio, TARGET_GPIO_PORT_SWITCH2))
            {
                bm_gpio_set(gpio, TARGET_GPIO_PORT_LED2);
            }
            if (bm_gpio_read(gpio, TARGET_GPIO_PORT_SWITCH3))
            {
                bm_gpio_set(gpio, TARGET_GPIO_PORT_LED3);
            }
        }
        else
        {
            bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED0);
            bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED1);
            bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED2);
            bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED3);
        }
    }
}

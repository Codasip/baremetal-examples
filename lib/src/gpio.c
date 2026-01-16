/* Copyright 2023-2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/gpio.h"

#include "baremetal/interrupt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool bm_gpio_read(bm_gpio_t *gpio, unsigned num)
{
    return gpio->regs->GPIO_DATA & (1UL << num);
}

void bm_gpio_set(bm_gpio_t *gpio, unsigned num)
{
    gpio->shadow |= (1UL << num);
    gpio->regs->GPIO_DATA = gpio->shadow;
}

void bm_gpio_clear(bm_gpio_t *gpio, unsigned num)
{
    gpio->shadow &= ~(1UL << num);
    gpio->regs->GPIO_DATA = gpio->shadow;
}

void bm_gpio_dir_set(bm_gpio_t *gpio, unsigned num, bm_gpio_dir_t dir)
{
    uint32_t bit = 1UL << num;

    if (dir == BM_GPIO_DIR_OUTPUT)
    {
        gpio->regs->GPIO_TRI &= ~(bit);
    }
    else
    {
        gpio->regs->GPIO_TRI |= bit;
    }
}

void bm_gpio_init_irq(bm_gpio_t *gpio)
{
    gpio->regs->GIER  = 0x80000000;
    gpio->regs->IPIER = 0x1;
    bm_gpio_clear_irq(gpio);
    bm_ext_irq_init();
    bm_ext_irq_enable(gpio->ext_irq_id);
}

void bm_gpio_set_irq(bm_gpio_t *gpio)
{
    if (!gpio->regs->IPISR)
    {
        gpio->regs->IPISR = 0x1;
    }
}

void bm_gpio_clear_irq(bm_gpio_t *gpio)
{
    if (gpio->regs->IPISR)
    {
        gpio->regs->IPISR = 0x1;
    }
}

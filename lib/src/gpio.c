/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/gpio.h"

#include "baremetal/interrupt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct bm_gpio_regs {
    volatile uint32_t GPIO_DATA;  ///< (0x0000) Channel 1 AXI GPIO Data Register
    volatile uint32_t GPIO_TRI;   ///< (0x0004) Channel 1 AXI GPIO 3-state Control Register
    volatile uint32_t GPIO2_DATA; ///< (0x0008) Channel 2 AXI GPIO Data Register
    volatile uint32_t GPIO2_TRI;  ///< (0x000C) Channel 2 AXI GPIO 3-state Control Register
    uint8_t           _reserved1[0x10C];
    volatile uint32_t GIER;  ///< (0x011C) Global Interrupt Enable Register
    volatile uint32_t IPISR; ///< (0x0120) IP Interrupt Status Register
    uint8_t           _reserved2[0x4];
    volatile uint32_t IPIER; ///< (0x0128) IP Interrupt Enable Register (IP IER)
};

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

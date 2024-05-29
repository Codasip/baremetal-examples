/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/platform.h"

#include "../unified_int_mem_map.h"
#include "baremetal/clint.h"
#include "baremetal/common.h"
#include "baremetal/gpio.h"
#include "baremetal/spi.h"
#include "baremetal/uart.h"
#include "baremetal/verbose.h"

/**
 * \brief Peripherals available on this target
 */
static bm_clint_t clint   = {.regs = (bm_clint_regs_t *)CLINT_ADDR, .freq = TARGET_CLK_FREQ};
static bm_uart_t  uart    = {.regs       = (bm_uart_regs_t *)UART_ADDR,
                             .ext_irq_id = UART_IRQ_ID,
                             .freq       = TARGET_CLK_FREQ};
static bm_spi_t   spi_sd  = {.regs = (bm_spi_regs_t *)SPI_SD_ADDR, .ext_irq_id = SPI_SD_IRQ_ID};
static bm_gpio_t  gpio_io = {.regs = (bm_gpio_regs_t *)GPIO_IO_ADDR, .ext_irq_id = GPIO_IO_IRQ_ID};
static bm_gpio_t  gpio_sd = {.regs = (bm_gpio_regs_t *)GPIO_SD_ADDR, .ext_irq_id = GPIO_SD_IRQ_ID};

void *target_peripheral_get(int id)
{
    switch (id)
    {
        case BM_PERIPHERAL_CLINT:
            return (void *)&clint;
        case BM_PERIPHERAL_UART_CONSOLE:
            return (void *)&uart;
        case BM_PERIPHERAL_SPI_SD:
            return (void *)&spi_sd;
        case BM_PERIPHERAL_GPIO_LEDS_SWITCHES:
            return (void *)&gpio_io;
        case BM_PERIPHERAL_GPIO_SD:
            return (void *)&gpio_sd;
        default:
            return NULL;
    }
}

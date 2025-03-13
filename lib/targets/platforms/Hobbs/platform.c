/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/platform.h"

#include "../unified_int_mem_map.h"
#include "baremetal/aead.h"
#include "baremetal/clic.h"
#include "baremetal/clint.h"
#include "baremetal/common.h"
#include "baremetal/gpio.h"
#include "baremetal/id_reg.h"
#include "baremetal/plic.h"
#include "baremetal/spi.h"
#include "baremetal/trng.h"
#include "baremetal/uart.h"
#include "baremetal/verbose.h"

/**
 * \brief Peripherals available on this target
 */
#ifdef TARGET_HAS_PLIC
static bm_plic_t plic = {.regs = (bm_plic_regs_t *)PLIC_ADDR};
#elif defined(TARGET_HAS_CLIC)
static bm_clic_t clic = {.regs = (bm_clic_regs_t *)CLIC_ADDR};
#endif
static bm_clint_t aclint = {.regs = (bm_clint_regs_t *)ACLINT_ADDR, .freq = TARGET_PLATFORM_FREQ};
static bm_uart_t  uart   = {.regs       = (bm_uart_regs_t *)UART_ADDR,
                            .ext_irq_id = UART_IRQ_ID,
                            .freq       = TARGET_PLATFORM_FREQ};
static bm_spi_t spi_flash = {.regs = (bm_spi_regs_t *)SPI_FLASH_ADDR, .ext_irq_id = SPI_FLASH_IRQ_ID};
static bm_spi_t    spi_sd  = {.regs = (bm_spi_regs_t *)SPI_SD_ADDR, .ext_irq_id = SPI_SD_IRQ_ID};
static bm_gpio_t   gpio_io = {.regs = (bm_gpio_regs_t *)GPIO_IO_ADDR, .ext_irq_id = GPIO_IO_IRQ_ID};
static bm_gpio_t   gpio_sd = {.regs = (bm_gpio_regs_t *)GPIO_SD_ADDR, .ext_irq_id = GPIO_SD_IRQ_ID};
static bm_id_reg_t id_reg  = {.regs = (bm_id_regs_t *)PLAT_ID_ADDR};
#ifdef CONFIG_SECURITY
static bm_trng_t trng = {.regs = (bm_trng_regs_t *)TRNG_ADDR};
static bm_aead_t aead = {.regs = (bm_aead_regs_t *)AEAD_ADDR};
#endif

void *target_peripheral_get(int id)
{
    switch (id)
    {
#ifdef TARGET_HAS_PLIC
        case BM_PERIPHERAL_PLIC:
            return (void *)&plic;
#elif defined(TARGET_HAS_CLIC)
        case BM_PERIPHERAL_CLIC:
            return (void *)&clic;
#endif
        case BM_PERIPHERAL_CLINT:
            return (void *)&aclint;
        case BM_PERIPHERAL_UART_CONSOLE:
            return (void *)&uart;
        case BM_PERIPHERAL_SPI_FLASH:
            return (void *)&spi_flash;
        case BM_PERIPHERAL_SPI_SD:
            return (void *)&spi_sd;
        case BM_PERIPHERAL_GPIO_LEDS_SWITCHES:
            return (void *)&gpio_io;
        case BM_PERIPHERAL_GPIO_SD:
            return (void *)&gpio_sd;
        case BM_PERIPHERAL_ID_REG:
            return (void *)&id_reg;
#ifdef CONFIG_SECURITY
        case BM_PERIPHERAL_AEAD:
            return (void *)&aead;
        case BM_PERIPHERAL_TRNG:
            return (void *)&trng;
#endif
        default:
            return NULL;
    }
}

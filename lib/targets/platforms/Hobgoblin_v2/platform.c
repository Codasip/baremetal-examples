/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/platform.h"

#include "baremetal/aead.h"
#include "baremetal/bm_cheri.h"
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

// Include Memory Map and Interrupt Map files for platform
#include "interrupt_map.h"
#include "memory_map.h"

/**
 * \brief Peripherals available on this target
 */
#ifdef TARGET_HAS_PLIC
    #ifndef TARGET_SIMULATOR
static bm_plic_t plic = {.regs = (bm_plic_regs_t *)PLIC_ADDR};
    #endif
#elif defined(TARGET_HAS_CLIC)
static bm_clic_t clic = {.regs = (bm_clic_regs_t *)CLIC_ADDR};
#endif
#ifndef TARGET_SIMULATOR
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
#endif

void *target_peripheral_get(int id)
{
    switch (id)
    {
#ifdef TARGET_HAS_PLIC
    #ifndef TARGET_SIMULATOR
        case BM_PERIPHERAL_PLIC:
            plic.regs = (bm_plic_regs_t *)addr_to_io_ptr((uintptr_t)PLIC_ADDR, sizeof(bm_plic_regs_t));
            return (void *)&plic;
    #endif
#elif defined(TARGET_HAS_CLIC)
        case BM_PERIPHERAL_CLIC:
            clic.regs = (bm_clic_regs_t *)addr_to_io_ptr((uintptr_t)CLIC_ADDR, sizeof(bm_clic_regs_t));
            return (void *)&clic;
#endif
#ifndef TARGET_SIMULATOR
        case BM_PERIPHERAL_CLINT:
            aclint.regs = (bm_clint_regs_t *)addr_to_io_ptr((uintptr_t)ACLINT_ADDR,
                                                            sizeof(bm_clint_regs_t));
            return (void *)&aclint;
        case BM_PERIPHERAL_UART_CONSOLE:
            uart.regs = (bm_uart_regs_t *)addr_to_io_ptr((uintptr_t)UART_ADDR, sizeof(bm_uart_regs_t));
            return (void *)&uart;
        case BM_PERIPHERAL_SPI_FLASH:
            spi_flash.regs = (bm_spi_regs_t *)addr_to_io_ptr((uintptr_t)SPI_FLASH_ADDR,
                                                             sizeof(bm_spi_regs_t));
            return (void *)&spi_flash;
        case BM_PERIPHERAL_SPI_SD:
            spi_sd.regs = (bm_spi_regs_t *)addr_to_io_ptr((uintptr_t)SPI_SD_ADDR,
                                                          sizeof(bm_spi_regs_t));
            return (void *)&spi_sd;
        case BM_PERIPHERAL_GPIO_LEDS_SWITCHES:
            gpio_io.regs = (bm_gpio_regs_t *)addr_to_io_ptr((uintptr_t)GPIO_IO_ADDR,
                                                            sizeof(bm_gpio_regs_t));
            return (void *)&gpio_io;
        case BM_PERIPHERAL_GPIO_SD:
            gpio_sd.regs = (bm_gpio_regs_t *)addr_to_io_ptr((uintptr_t)GPIO_SD_ADDR,
                                                            sizeof(bm_gpio_regs_t));
            return (void *)&gpio_sd;
        case BM_PERIPHERAL_ID_REG:
            id_reg.regs = (bm_id_regs_t *)addr_to_io_ptr((uintptr_t)PLAT_ID_ADDR, sizeof(bm_id_regs_t));
            return (void *)&id_reg;
    #ifdef CONFIG_SECURITY
        case BM_PERIPHERAL_AEAD:
            return (void *)&aead;
        case BM_PERIPHERAL_TRNG:
            return (void *)&trng;
    #endif
#endif
        default:
            bm_fatal("requested unknown peripheral %d", id);
            return NULL;
    }
}

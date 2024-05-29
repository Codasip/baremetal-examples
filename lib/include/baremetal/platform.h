/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_PLATFORM_H
#define BAREMETAL_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Enumeration of available peripherals */
typedef enum {
    BM_PERIPHERAL_CLINT,              // CLINT peripheral
    BM_PERIPHERAL_PLIC,               // PLIC peripheral
    BM_PERIPHERAL_UART_CONSOLE,       // UART connected to on-board USB UART bridge
    BM_PERIPHERAL_SPI_FLASH,          // SPI connected to on-board SPI FLASH
    BM_PERIPHERAL_SPI_SD,             // SPI connected to on-board SD card slot
    BM_PERIPHERAL_GPIO_LEDS_SWITCHES, // GPIO connected to on-board LEDs and switches
    BM_PERIPHERAL_GPIO_SD,            // GPIO connected to on-board SD card slot
    BM_PERIPHERAL_I2C_PWR,            // I2C connected to on-board power monitoring sensors
    BM_PERIPHERAL_TRNG,               // TRNG peripheral
    BM_PERIPHERAL_AEAD,               // AEAD peripheral
    BM_PERIPHERAL_ID_REG,             // PLatform ID register
} bm_peripheral_id;

/**
 * \brief Get pointer to peripheral of given type, function is defined in the target directory
 *
 * \param id ID of the requested peripheral, bm_peripheral_id or target defined custom value
 *
 * \return Pointer to requested peripheral, or NULL if not available for this target
 */
void *target_peripheral_get(int id);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_PLATFORM_H */

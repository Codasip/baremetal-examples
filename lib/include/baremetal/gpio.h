/* Copyright 2023-2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_GPIO_H
#define BAREMETAL_GPIO_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/** \brief Structure describing GPIO peripheral registers */
typedef struct bm_gpio_regs bm_gpio_regs_t;

/** \brief Structure holding data neccessary to service the GPIO peripheral */
typedef struct {
    struct bm_gpio_regs *regs;       ///< Pointer to the peripheral registers
    uint32_t             shadow;     ///< Shadow copy of output pins
    unsigned             ext_irq_id; ///< External interrupt identifier
} bm_gpio_t;

/** \brief Pin direction definitions */
typedef enum {
    BM_GPIO_DIR_OUTPUT,
    BM_GPIO_DIR_INPUT,
} bm_gpio_dir_t;

/**
 * \brief Function for reading input level of a GPIO pin on the given port
 *
 * \param gpio Pointer to the structure of registers of the peripheral
 * \param num  Specifies the pin number to be read
 *
 * \return True if the level is high, false if the level is low
 */
bool bm_gpio_read(bm_gpio_t *gpio, unsigned num);

/**
 * \brief Function for setting high output level of a GPIO pin on the given port
 *
 * \param gpio Pointer to the structure of registers of the peripheral
 * \param num  Specifies the pin number to be set
 */
void bm_gpio_set(bm_gpio_t *gpio, unsigned num);

/**
 * \brief Function for setting low output level of a GPIO pin on the given port
 *
 * \param gpio Pointer to the structure of registers of the peripheral
 * \param num  Specifies the pin number to be cleared
 */
void bm_gpio_clear(bm_gpio_t *gpio, unsigned num);

/**
 * \brief Function for setting direction of a GPIO pin on the given port
 *
 * \param gpio Pointer to the structure of registers of the peripheral
 * \param num  Specifies the pin number to be cleared
 * \param dir  Direction of the pin (input or output)
 */
void bm_gpio_dir_set(bm_gpio_t *gpio, unsigned num, bm_gpio_dir_t dir);

/**
 * \brief Function for enabling generation of GPIO interrupts
 *
 * \param gpio Pointer to the structure of registers of the peripheral
 */
void bm_gpio_init_irq(bm_gpio_t *gpio);

/**
 * \brief Function for setting GPIO interrupt
 *
 * \param gpio Pointer to the structure of registers of the peripheral
 */
void bm_gpio_set_irq(bm_gpio_t *gpio);

/**
 * \brief Function for clearing GPIO interrupt
 *
 * \param gpio Pointer to the structure of registers of the peripheral
 */
void bm_gpio_clear_irq(bm_gpio_t *gpio);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_GPIO_H */

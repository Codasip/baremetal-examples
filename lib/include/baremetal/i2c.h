/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_I2C_H
#define BAREMETAL_I2C_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Structure describing I2C peripheral registers */
typedef struct bm_i2c_regs bm_i2c_regs_t;

/** \brief Structure holding data neccessary to service the I2C peripheral */
typedef struct {
    bm_i2c_regs_t *regs;       ///< Pointer to the peripheral registers
    unsigned       ext_irq_id; ///< External interrupt identifier
} bm_i2c_t;

/**
 * \brief Initialize I2C device
 *
 * \param i2c I2C device
 */
void bm_i2c_init(bm_i2c_t *i2c);

/**
 * \brief Transmit data over the I2C bus
 *
 * \param i2c I2C device to write to
 * \param address Slave address
 * \param buffer Data to send
 * \param length Length of the data
 * \param stop_bit If set, generate stop condition
 */
void bm_i2c_send(bm_i2c_t *i2c, uint8_t address, const uint8_t *buffer, size_t length, bool stop_bit);

/**
 * \brief Receive data over the I2C bus
 *
 * \param i2c I2C device to read from
 * \param address Slave address
 * \param buffer Output buffer
 * \param length Length of the data
 * \param stop_bit If set, generate stop condition
 */
void bm_i2c_recv(bm_i2c_t *i2c, uint8_t address, uint8_t *buffer, size_t length, bool stop_bit);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_I2C_H */

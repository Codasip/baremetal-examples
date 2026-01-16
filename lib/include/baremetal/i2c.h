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

/** \brief Xilinx IIC IP registers */
struct bm_i2c_regs {
    uint8_t           _reserved1[0x1C];
    volatile uint32_t GIE; ///< (0x001c) Global Interrupt Enable
    volatile uint32_t ISR; ///< (0x0020) Interrupt Status Register
    uint8_t           _reserved2[0x4];
    volatile uint32_t IER; ///< (0x0028) Interrupt Enable Register
    uint8_t           _reserved3[0x14];
    volatile uint32_t SRS; ///< (0x0040) Soft Reset Register
    uint8_t           _reserved4[0xbc];
    volatile uint32_t CR;        ///< (0x0100) Control Register
    volatile uint32_t SR;        ///< (0x0104) Status Register
    volatile uint32_t TX_FIFO;   ///< (0x0108) Transmit FIFO Register
    volatile uint32_t RX_FIFO;   ///< (0x010C) Receive FIFO Register
    volatile uint32_t ADR;       ///< (0x0110) Slave Address Register
    volatile uint32_t TX_OCY;    ///< (0x0114) Tx FIFO Occupancy Register
    volatile uint32_t RX_OCY;    ///< (0x0118) Rx FIFO Occupancy Register
    volatile uint32_t TEN_ADR;   ///< (0x011C) Slave Ten Bit Address Register
    volatile uint32_t RX_PDIR;   ///< (0x0120) Rx FIFO Programmable Depth Interrupt Register
    volatile uint32_t TX_GPO;    ///< (0x0124) General Purpose Output Register
    volatile uint32_t TX_TSUSTA; ///< (0x0128) Timing Parameter Register
    volatile uint32_t TX_TSUSTO; ///< (0x012C) Timing Parameter Register
    volatile uint32_t TX_THDSTA; ///< (0x0130) Timing Parameter Register
    volatile uint32_t TX_TSUDAT; ///< (0x0134) Timing Parameter Register
    volatile uint32_t TX_TBUF;   ///< (0x0138) Timing Parameter Register
    volatile uint32_t TX_THIGH;  ///< (0x013C) Timing Parameter Register
    volatile uint32_t TX_TLOW;   ///< (0x0140) Timing Parameter Register
    volatile uint32_t TX_THDDAT; ///< (0x0144) Timing Parameter Register
};

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

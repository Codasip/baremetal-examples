/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/i2c.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

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

#define ISR_BIT_TX_EMPTY     (1 << 2)

#define SRS_RESET_MASK       0xA

#define CR_BIT_IIC_ENABLE    (1 << 0)
#define CR_BIT_MSMS_MASTER   (1 << 2)
#define CR_BIT_MODE_TRANSMIT (1 << 3)
#define CR_BIT_MODE_RECEIVE  (0 << 3)
#define CR_BIT_NACK          (1 << 4)
#define CR_BIT_RSTART        (1 << 5)

#define SR_BIT_BUS_BUSY      (1 << 2)
#define SR_BIT_RX_EMPTY      (1 << 6)

void bm_i2c_init(bm_i2c_t *i2c)
{
    // Enable the device
    i2c->regs->CR = CR_BIT_IIC_ENABLE;
    // Soft reset
    i2c->regs->SRS = SRS_RESET_MASK;
}

void bm_i2c_send(bm_i2c_t *i2c, uint8_t address, const uint8_t *buffer, size_t length, bool stop_bit)
{
    // Generate repeated start condition
    if (i2c->regs->CR & CR_BIT_MSMS_MASTER)
    {
        i2c->regs->CR |= CR_BIT_RSTART;
    }

    // Select slave and write mode
    i2c->regs->TX_FIFO = address << 1;

    // Wait for repeated start to generate and the flag to clear
    while (i2c->regs->CR & CR_BIT_RSTART)
        ;

    // Configure for master trasmitter
    i2c->regs->CR = CR_BIT_IIC_ENABLE | CR_BIT_MSMS_MASTER | CR_BIT_MODE_TRANSMIT;

    // Wait for start condition
    while (!(i2c->regs->CR & CR_BIT_MSMS_MASTER))
        ;

    // Clear transmit fifo empty interrupt
    if (i2c->regs->ISR & ISR_BIT_TX_EMPTY)
    {
        i2c->regs->ISR = ISR_BIT_TX_EMPTY;
    }

    // Fill FIFO
    for (size_t i = 0; i < (stop_bit ? length - 1 : length); ++i)
    {
        i2c->regs->TX_FIFO = buffer[i];
    }

    // Wait until fifo empty
    while (!(i2c->regs->ISR & ISR_BIT_TX_EMPTY))
        ;

    // Generate stop condition and send last byte
    if (stop_bit)
    {
        i2c->regs->CR &= ~CR_BIT_MSMS_MASTER;
        i2c->regs->TX_FIFO = buffer[length - 1];

        // Wait for stop condition
        while (i2c->regs->SR & SR_BIT_BUS_BUSY)
            ;
    }
}

void bm_i2c_recv(bm_i2c_t *i2c, uint8_t address, uint8_t *buffer, size_t length, bool stop_bit)
{
    // Generate repeated start condition
    if (i2c->regs->CR & CR_BIT_MSMS_MASTER)
    {
        i2c->regs->CR |= CR_BIT_RSTART;
    }

    // Select slave and read mode
    i2c->regs->TX_FIFO = (address << 1) | 1;

    // Wait for repeated start to generate and the flag to clear
    while (i2c->regs->CR & CR_BIT_RSTART)
        ;

    // Configure for master receiver
    i2c->regs->CR = CR_BIT_IIC_ENABLE | CR_BIT_MSMS_MASTER | CR_BIT_MODE_RECEIVE;

    // Wait for start condition
    while (!(i2c->regs->CR & CR_BIT_MSMS_MASTER))
        ;

    for (size_t i = 0; i < length - 1; ++i)
    {
        // Wait for data
        while (i2c->regs->SR & SR_BIT_RX_EMPTY)
            ;

        buffer[i] = i2c->regs->RX_FIFO;
    }

    // Not acknowledge the last byte
    i2c->regs->CR |= CR_BIT_NACK;

    // Generate stop condition
    if (stop_bit)
    {
        i2c->regs->CR &= ~CR_BIT_MSMS_MASTER;
    }

    // Wait for data
    while (i2c->regs->SR & SR_BIT_RX_EMPTY)
        ;

    buffer[length - 1] = i2c->regs->RX_FIFO;

    if (stop_bit)
    {
        // Wait for stop condition
        while (i2c->regs->SR & SR_BIT_BUS_BUSY)
            ;
    }
}

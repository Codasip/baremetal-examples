/* Copyright 2023-2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/i2c.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

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

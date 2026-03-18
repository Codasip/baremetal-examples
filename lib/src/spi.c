/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/spi.h"

#include "baremetal/interrupt.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define CR_BIT_LOOP         (1 << 0)
#define CR_BIT_SPE          (1 << 1)
#define CR_BIT_MASTER       (1 << 2)
#define CR_BIT_CPOL         (1 << 3)
#define CR_BIT_CPHA         (1 << 4)
#define CR_BIT_TXFIFO_RESET (1 << 5)
#define CR_BIT_RXFIFO_RESET (1 << 6)
#define CR_BIT_MANUAL_SSEL  (1 << 7)
#define CR_BIT_INHIBIT      (1 << 8)

#define SR_BIT_RX_EMPTY     (1 << 0)
#define SR_BIT_RX_FULL      (1 << 1)
#define SR_BIT_TX_EMPTY     (1 << 2)
#define SR_BIT_TX_FULL      (1 << 3)

#define SSEL_DEASSERT_ALL   0xFFFFFFFF
#define SSEL_ASSERT_0       0xFFFFFFFE

void bm_spi_reset(bm_spi_t *spi)
{
    bm_spi_cs_deassert(spi);

    spi->regs->SRR = 0x0000000a; /* Software reset SPI */
}

void bm_spi_init(bm_spi_t *spi)
{
    bm_spi_cs_deassert(spi);

    spi->regs->SPICR = CR_BIT_MANUAL_SSEL | CR_BIT_MASTER | CR_BIT_SPE | CR_BIT_TXFIFO_RESET |
                       CR_BIT_RXFIFO_RESET;
}

void bm_spi_cs_assert(bm_spi_t *spi)
{
    spi->regs->SPISSR = SSEL_ASSERT_0;
}

void bm_spi_cs_deassert(bm_spi_t *spi)
{
    spi->regs->SPISSR = SSEL_DEASSERT_ALL;
}

void bm_spi_txrx_bufs(bm_spi_t *spi, const uint8_t *txbuf, uint8_t *rxbuf, size_t size)
{
    for (unsigned i = 0; i < size; ++i)
    {
        spi->regs->SPIDTR = txbuf ? txbuf[i] : 0xff;

        while (spi->regs->SPISR & SR_BIT_RX_EMPTY)
            ;

        if (rxbuf)
        {
            rxbuf[i] = spi->regs->SPIDRR;
        }
        else
        {
            (void)spi->regs->SPIDRR;
        }
    }
}

void bm_spi_write_byte(bm_spi_t *spi, uint8_t data)
{
    bm_spi_txrx_bufs(spi, &data, NULL, 1);
}

uint8_t bm_spi_read_byte(bm_spi_t *spi)
{
    uint8_t data;
    bm_spi_txrx_bufs(spi, NULL, &data, 1);
    return data;
}

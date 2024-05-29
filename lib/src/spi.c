/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/spi.h"

#include "baremetal/interrupt.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/** \brief Xilinx Quad SPI IP registers */
struct bm_spi_regs {
    uint8_t           _reserved1[0x1C];
    volatile uint32_t DGIER; ///< (0x001C) Device global interrupt enable register
    volatile uint32_t IPISR; ///< (0x0020) IP interrupt status register
    uint8_t           _reserved2[0x4];
    volatile uint32_t IPIER; ///< (0x0028) IP interrupt enable register
    uint8_t           _reserved3[0x14];
    volatile uint32_t SRR; ///< (0x0040) Software reset register
    uint8_t           _reserved4[0x1C];
    volatile uint32_t SPICR;  ///< (0x0060) SPI control register
    volatile uint32_t SPISR;  ///< (0x0064) SPI status register
    volatile uint32_t SPIDTR; ///< (0x0068) SPI data transmit register
    volatile uint32_t SPIDRR; ///< (0x006C) SPI data receive register
    volatile uint32_t SPISSR; ///< (0x0070) SPI Slave select register
    volatile uint32_t TFOCC;  ///< (0x0074) Transmit FIFO occupancy register
    volatile uint32_t RFOCC;  ///< (0x0078) Receive FIFO occupancy register
};

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

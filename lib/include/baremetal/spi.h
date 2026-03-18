/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_SPI_H
#define BAREMETAL_SPI_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/** \brief Structure describing SPI peripheral registers */
typedef struct bm_spi_regs bm_spi_regs_t;

/** \brief Structure holding data neccessary to service the SPI peripheral. */
typedef struct {
    bm_spi_regs_t *regs;       ///< Pointer to the peripheral registers
    unsigned       ext_irq_id; ///< External interrupt identifier
} bm_spi_t;

/**
 * \brief Assert chip select
 *
 * \param spi SPI device
 */
void bm_spi_cs_assert(bm_spi_t *spi);

/**
 * \brief Deassert chip select
 *
 * \param spi SPI device
 */
void bm_spi_cs_deassert(bm_spi_t *spi);

/**
 * \brief Write a byte to the SPI bus
 *
 * \param spi SPI device to write to
 * \param data Data to write
 */
void bm_spi_write_byte(bm_spi_t *spi, uint8_t data);

/**
 * \brief Read a byte from the SPI bus
 *
 * \param spi SPI device to read from
 *
 * \return Data read
 */
uint8_t bm_spi_read_byte(bm_spi_t *spi);

void bm_spi_txrx_bufs(bm_spi_t *spi, const uint8_t *txbuf, uint8_t *rxbuf, size_t size);

/**
 * \brief Initialize SPI controller device
 *
 * \param spi Pointer to the SPI device to initialize
 */
void bm_spi_init(bm_spi_t *spi);

/**
 * \brief Reset the SPI controller device
 *
 * \param spi Pointer to the SPI device to initialize
 */
void bm_spi_reset(bm_spi_t *spi);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_SPI_H */

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef CODASIP_FLASH_H
#define CODASIP_FLASH_H

#include <baremetal/spi.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Structure holding data neccessary to service the SPI flash device. */
typedef struct {
    bm_spi_t *spi; ///< Pointer to structure describing the SPI peripheral used
} spi_flash_t;

/**
 * \brief Initialize S25FL128S FLASH device
 *
 * \param flash Pointer to the flash structure to initialize
 * \param spi Spi controller device to use
 */
void flash_init(spi_flash_t *flash, bm_spi_t *spi);

/**
 * \brief Read data from S25FL128S FLASH device
 *
 * \param flash FLASH device to read from
 * \param address Address in the FLASH memory to start the read at
 * \param length Number of bytes to read
 * \param buffer Buffer to store the data in
 */
void flash_read(spi_flash_t *flash, uint32_t address, uint32_t length, uint8_t *buffer);

/**
 * \brief Write data to S25FL128S FLASH device
 *
 *  This function takes care of reading out the previous contents of the erase sector,
 *  erasing it and writing data back, so the rest of the memory stays unchanged.
 *
 * \param flash FLASH device to write to
 * \param address Address in the FLASH memory to start the write at
 * \param length Number of bytes to write
 * \param buffer Data to write
 *
 * \return Zero on success non-zero otherwise
 */
int flash_write(spi_flash_t *flash, uint32_t address, uint32_t count, uint8_t *buffer);

/**
 * \brief Program page of S25FL128S FLASH device
 *
 *  WARNING: This function does not take care of the content previously written on the flash, i.e.
 *  it does not erase the sectors. If you're not sure about what you're doing, use \ref flash_write instead.
 *
 * \param flash FLASH device to write to
 * \param address Address in the FLASH memory to start the write at
 * \param length Number of bytes to write
 * \param buffer Data to write
 *
 * \return Zero on success non-zero otherwise
 */
int flash_page_program(spi_flash_t *flash, uint32_t address, uint32_t length, uint8_t *buffer);

/**
 * \brief Erase an erase sector of S25FL128S FLASH device
 *
 * \param flash FLASH device to work with
 * \param address Address in the erase sector to erase
 *
 * \return Zero on success non-zero otherwise
 */
int flash_sector_erase(spi_flash_t *flash, uint32_t address);

#ifdef __cplusplus
}
#endif

#endif // CODASIP_FLASH_H

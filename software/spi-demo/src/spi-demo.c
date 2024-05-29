/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "s25fl128s.h"

#include <baremetal/platform.h>
#include <baremetal/spi.h>
#include <baremetal/time.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * \brief SPI Master demo
 *
 * Reads out the counter from the flash memory, increments it and writes back.
 */
int main(void)
{
    puts("Welcome to the SPI demo!\n");

    bm_spi_t   *spi = (bm_spi_t *)target_peripheral_get(BM_PERIPHERAL_SPI_FLASH);
    spi_flash_t flash;

    bm_spi_init(spi);
    flash_init(&flash, spi);

    uint32_t tmp     = 0;
    uint8_t  counter = 0;

    // Check magic word
    flash_read(&flash, 0x0, 3, (uint8_t *)&tmp);

    if (tmp == 0xC0FFEE)
    {
        // If the magic word is present, read out the counter
        flash_read(&flash, 0x3, 1, &counter);
    }
    else
    {
        // Otherwise write the magic word for the next run
        tmp = 0xC0FFEE;
        flash_write(&flash, 0x0, 3, (uint8_t *)&tmp);
    }

    printf("This demo ran %u times before.\n", counter);

    // Increment counter of runs
    ++counter;
    flash_write(&flash, 0x3, 1, (uint8_t *)&counter);

    puts("Bye.");
    return EXIT_SUCCESS;
}

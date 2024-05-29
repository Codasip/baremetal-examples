/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "s25fl128s.h"

#include <stdlib.h>
#include <string.h>

#define FLASH_STATUS_WIP        0x1  // Work in progress
#define FLASH_STATUS_E_ERR      0x20 // Erase error
#define FLASH_STATUS_P_ERR      0x40 // Programming error
#define FLASH_ERASE_SECTOR_SIZE 4096
#define FLASH_PAGE_PROGRAM_SIZE 256

#define FLASH_OP_WRR            0x1  // Write Register
#define FLASH_OP_RDSR           0x5  // Read Status Register
#define FLASH_OP_RDCR           0x35 // Read Configuration Register
#define FLASH_OP_WREN           0x6  // Write Enable
#define FLASH_OP_4PP            0x12 // Page Programming
#define FLASH_OP_4QPP           0x34 // Quad Page Programming
#define FLASH_OP_4READ          0x13 // Read Data bytes
#define FLASH_OP_4DOR           0x3C // Read Dual Out
#define FLASH_OP_4QOR           0x6C // Read Quad Out
#define FLASH_OP_4P4E           0x21 // 4 KB Parameter Sector Erase
#define FLASH_OP_RST            0xF0 // Software Reset

static inline void flash_cmd_set_op(spi_flash_t *flash, uint8_t op)
{
    bm_spi_write_byte(flash->spi, op);
}

static inline void flash_cmd_set_address(spi_flash_t *flash, uint32_t address)
{
    uint8_t address_buffer[4];

    address_buffer[0] = address >> 24;
    address_buffer[1] = address >> 16;
    address_buffer[2] = address >> 8;
    address_buffer[3] = address >> 0;

    bm_spi_txrx_bufs(flash->spi, address_buffer, NULL, 4);
}

static inline uint32_t flash_get_status(spi_flash_t *flash)
{
    bm_spi_cs_assert(flash->spi);

    flash_cmd_set_op(flash, FLASH_OP_RDSR);
    uint32_t status = bm_spi_read_byte(flash->spi);

    bm_spi_cs_deassert(flash->spi);
    return status;
}

static inline void flash_simple_cmd(spi_flash_t *flash, unsigned command)
{
    bm_spi_cs_assert(flash->spi);
    flash_cmd_set_op(flash, command);
    bm_spi_cs_deassert(flash->spi);

    // Wait for completion
    while (flash_get_status(flash) & FLASH_STATUS_WIP)
        ;
}

int flash_sector_erase(spi_flash_t *flash, uint32_t address)
{
    flash_simple_cmd(flash, FLASH_OP_WREN);

    bm_spi_cs_assert(flash->spi);

    flash_cmd_set_op(flash, FLASH_OP_4P4E);
    flash_cmd_set_address(flash, address);

    bm_spi_cs_deassert(flash->spi);

    // Wait for completion
    while (flash_get_status(flash) & FLASH_STATUS_WIP)
        ;

    return (flash_get_status(flash) & FLASH_STATUS_E_ERR) ? -1 : 0;
}

int flash_page_program(spi_flash_t *flash, uint32_t address, uint32_t length, uint8_t *buffer)
{
    while (length > 0)
    {
        // Calculate bytes left to the page boundary
        uint16_t write_size = FLASH_PAGE_PROGRAM_SIZE - address % FLASH_PAGE_PROGRAM_SIZE;

        // If we have less data to write that the rest of the page
        if (length < write_size)
            write_size = length;

        flash_simple_cmd(flash, FLASH_OP_WREN);

        bm_spi_cs_assert(flash->spi);

        flash_cmd_set_op(flash, FLASH_OP_4PP);
        flash_cmd_set_address(flash, address);
        bm_spi_txrx_bufs(flash->spi, buffer, NULL, write_size);

        bm_spi_cs_deassert(flash->spi);

        // Wait for completion
        while (flash_get_status(flash) & FLASH_STATUS_WIP)
            ;

        if (flash_get_status(flash) & FLASH_STATUS_P_ERR)
        {
            return -1;
        }

        length -= write_size;
        address += write_size;
        buffer += write_size;
    }

    return 0;
}

void flash_init(spi_flash_t *flash, bm_spi_t *spi)
{
    flash->spi = spi;

    flash_simple_cmd(flash, FLASH_OP_RST);
}

void flash_read(spi_flash_t *flash, uint32_t address, uint32_t length, uint8_t *buffer)
{
    bm_spi_cs_assert(flash->spi);

    flash_cmd_set_op(flash, FLASH_OP_4READ);
    flash_cmd_set_address(flash, address);
    bm_spi_txrx_bufs(flash->spi, NULL, buffer, length);

    bm_spi_cs_deassert(flash->spi);
}

int flash_write(spi_flash_t *flash, uint32_t address, uint32_t length, uint8_t *buffer)
{
    uint8_t  erase_sector_buffer[FLASH_ERASE_SECTOR_SIZE];
    uint32_t buffer_pos = 0;

    while (length > 0)
    {
        uint32_t sector_start_addr = address - address % FLASH_ERASE_SECTOR_SIZE;
        uint32_t curr_length       = FLASH_ERASE_SECTOR_SIZE;
        uint8_t *curr_buffer       = buffer + buffer_pos;

        /* The write is not aligned to erase sector boundary and/or
         * is shorter than the size of the erase sector.
         * It is necessary to first read out the contents.
         */
        if (address != sector_start_addr || length < FLASH_ERASE_SECTOR_SIZE)
        {
            uint32_t sector_offset = address - sector_start_addr;

            curr_length -= sector_offset;
            if (curr_length > length)
                curr_length = length;

            flash_read(flash, sector_start_addr, FLASH_ERASE_SECTOR_SIZE, erase_sector_buffer);

            memcpy(erase_sector_buffer + sector_offset, curr_buffer, curr_length);
            curr_buffer = erase_sector_buffer;
        }

        if (flash_sector_erase(flash, sector_start_addr))
        {
            return -1;
        }
        if (flash_page_program(flash, sector_start_addr, FLASH_ERASE_SECTOR_SIZE, curr_buffer))
        {
            return -2;
        }

        address += curr_length;
        buffer_pos += curr_length;
        length -= curr_length;
    }

    return 0;
}

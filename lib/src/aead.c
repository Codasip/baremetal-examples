/* Copyright 2024-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/aead.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define STATUS_KEY_SIZE_OFFSET    0
#define STATUS_KEY_SIZE_MASK      (0xf << STATUS_KEY_SIZE_OFFSET)
#define STATUS_BLOCK_SIZE_OFFSET  4
#define STATUS_BLOCK_SIZE_MASK    (0xf << STATUS_BLOCK_SIZE_OFFSET)
#define STATUS_NONCE_SIZE_OFFSET  8
#define STATUS_NONCE_SIZE_MASK    (0xf << STATUS_NONCE_SIZE_OFFSET)
#define STATUS_TAG_SIZE_OFFSET    12
#define STATUS_TAG_SIZE_MASK      (0xf << STATUS_TAG_SIZE_OFFSET)

#define FIFO_STATUS_INPUT_OFFSET  0
#define FIFO_STATUS_INPUT_MASK    (0xffffU << FIFO_STATUS_INPUT_OFFSET)
#define FIFO_STATUS_OUTPUT_OFFSET 16
#define FIFO_STATUS_OUTPUT_MASK   (0xffffU << FIFO_STATUS_OUTPUT_OFFSET)

#define CONFIG_MODE_ENC           0x1
#define CONFIG_MODE_DEC           0x0

#define COMMAND_CONFIG_SET        0x1
#define COMMAND_FINALIZE          0x2
#define COMMAND_RESET             0x3

/**
 * \brief Function for copying data into AEAD core registers, using 32-bit writes and zero-padded data
 *
 * \param registers AEAD registers to write
 * \param data Data to write
 * \param len Length of the data
 */
static void aead_copy_data_in(volatile uint32_t registers[], const uint8_t *data, unsigned len)
{
    unsigned reg_count = (len + 3) / 4;
    for (unsigned i = 0; i < reg_count; i++)
    {
        uint32_t tmp = 0;
        memcpy(&tmp, data + 4 * i, 4);
        registers[i] = tmp;
    }
}

/**
 * \brief Function for copying data out from AEAD core registers, using 32-bit reads
 *
 * \param data Buffer to store the read data
 * \param registers AEAD registers to read
 * \param len Length of the data
 */
static void aead_copy_data_out(uint8_t *data, const volatile uint32_t registers[], unsigned len)
{
    unsigned reg_count = (len + 3) / 4;
    for (unsigned i = 0; i < reg_count; i++)
    {
        uint32_t tmp = registers[i];
        memcpy(data + 4 * i, &tmp, 4);
    }
}

// Suported AEAD configuration options
static const unsigned aead_block_size_table[] = {8, 16};
static const unsigned aead_key_size_table[]   = {16, 20, 24, 32};
static const unsigned aead_nonce_size_table[] = {2, 3, 4, 6, 8, 12, 16};
static const unsigned aead_tag_size_table[]   = {4, 6, 8, 10, 12, 14, 16};

void bm_aead_init(bm_aead_t *aead)
{
    // Read AEAD parameters
    aead->block_size =
        aead_block_size_table[(aead->regs->STATUS & STATUS_BLOCK_SIZE_MASK) >> STATUS_BLOCK_SIZE_OFFSET];
    aead->key_size =
        aead_key_size_table[(aead->regs->STATUS & STATUS_KEY_SIZE_MASK) >> STATUS_KEY_SIZE_OFFSET];
    aead->nonce_size =
        aead_nonce_size_table[(aead->regs->STATUS & STATUS_NONCE_SIZE_MASK) >> STATUS_NONCE_SIZE_OFFSET];
    aead->tag_size =
        aead_tag_size_table[(aead->regs->STATUS & STATUS_TAG_SIZE_MASK) >> STATUS_TAG_SIZE_OFFSET];

    aead->regs->COMMAND = COMMAND_RESET;
}

/**
 * \brief Function for operating AEAD core FIFOs
 *
 * \param aead Pointer to AEAD core structure
 * \param data_in Data to write
 * \param data_out Buffer to store data read out, use NULL if no output is expected
 * \param len Length of the data
 */
static void aead_data_in_out(bm_aead_t *aead, const uint8_t *data_in, uint8_t *data_out, uint32_t len)
{
    unsigned blocks_written_in = 0;
    unsigned blocks_read_out   = 0;
    unsigned data_num_blocks   = (len + aead->block_size - 1) / aead->block_size;

    // Repeat until all data is processed
    while (data_out ? (blocks_read_out < data_num_blocks) : (blocks_written_in < data_num_blocks))
    {
        unsigned fifo_avail = (aead->regs->FIFO_STATUS & FIFO_STATUS_INPUT_MASK) >>
                              FIFO_STATUS_INPUT_OFFSET;
        unsigned blocks_ready = fifo_avail < (data_num_blocks - blocks_written_in)
                                    ? fifo_avail
                                    : data_num_blocks - blocks_written_in;
        while (blocks_ready > 0)
        {
            // Write a single block
            aead_copy_data_in(aead->regs->BLOCK_IN,
                              data_in + blocks_written_in * aead->block_size,
                              aead->block_size);
            blocks_ready--;
            blocks_written_in++;
        }

        if (data_out)
        {
            fifo_avail = (aead->regs->FIFO_STATUS & FIFO_STATUS_OUTPUT_MASK) >>
                         FIFO_STATUS_OUTPUT_OFFSET;
            blocks_ready = fifo_avail < (data_num_blocks - blocks_read_out)
                               ? fifo_avail
                               : data_num_blocks - blocks_read_out;
            while (blocks_ready > 0)
            {
                // Read out a single block
                aead_copy_data_out(data_out + blocks_read_out * aead->block_size,
                                   aead->regs->DATA_OUT,
                                   aead->block_size);
                blocks_ready--;
                blocks_read_out++;
            }
        }
    };
}

void bm_aead_run(bm_aead_t *aead, bm_aead_command_t *command)
{
    // Configure AEAD core
    aead->regs->CONFIG = command->decrypt ? CONFIG_MODE_DEC : CONFIG_MODE_ENC;

    aead->regs->PAYLOAD_SIZE = command->payload_size;
    aead->regs->AD_SIZE      = command->ad_size;

    aead_copy_data_in(aead->regs->KEY, command->key, aead->key_size);
    aead_copy_data_in(aead->regs->NONCE, command->nonce, aead->nonce_size);

    aead->regs->COMMAND = COMMAND_CONFIG_SET;

    // Process data
    aead_data_in_out(aead, command->additional_data, NULL, command->ad_size);
    aead_data_in_out(aead, command->payload, command->data_out, command->payload_size);
    aead_copy_data_out(command->tag, aead->regs->TAG, aead->tag_size);

    // Reset AEAD
    aead->regs->COMMAND = COMMAND_FINALIZE;
}

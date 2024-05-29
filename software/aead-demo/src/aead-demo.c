/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/aead.h>
#include <baremetal/platform.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static bool test_aead_command(bm_aead_t *aead, bm_aead_command_t *command)
{
    // Run the command
    bm_aead_run(aead, command);

    // Check results, assuming XOR fake hash function
    uint8_t block[16];
    for (unsigned i = 0; i < aead->block_size; ++i)
    {
        block[i] = command->key[i] ^ command->nonce[i] ^ command->additional_data[i];
    }

    bool ok = true;
    for (unsigned i = 0; i < command->payload_size / aead->block_size; ++i)
    {
        for (unsigned j = 0; j < aead->block_size; ++j)
        {
            block[j] ^= command->payload[i * aead->block_size + j];

            if (block[j] != command->data_out[i * aead->block_size + j])
            {
                printf("Data missmatch @%u Expected: 0x%x Got: 0x%x\n",
                       i * aead->block_size + j,
                       block[j],
                       command->data_out[i * aead->block_size + j]);
                ok = false;
            }

            if (command->decrypt)
            {
                // When decrypting, cyphertext is passed to the next round
                block[j] = command->payload[i * aead->block_size + j];
            }
        }
    }

    for (unsigned i = 0; i < aead->tag_size; ++i)
    {
        block[i] ^= command->key[i];

        if (block[i] != command->tag[i])
        {
            printf("Tag missmatch @%u Expected: 0x%x Got: 0x%x\n", i, block[i], command->tag[i]);
            ok = false;
        }
    }

    return ok;
}

int main(void)
{
    puts("Welcome to the AEAD demo.");

    bm_aead_t *aead = target_peripheral_get(BM_PERIPHERAL_AEAD);

    bm_aead_init(aead);

    uint8_t key[16] =
        {0xbe, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee};
    uint8_t nonce[16] =
        {0xc0, 0xff, 0xee, 0xc0, 0xff, 0xee, 0xc0, 0xff, 0xee, 0xc0, 0xff, 0xee, 0xc0, 0xff, 0xee, 0xc0};
    uint8_t ad[16] =
        {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t payload[128];
    uint8_t data_out[128];
    uint8_t data_out_dec[128];
    uint8_t tag[16];

    for (int i = 0; i < 128; ++i)
    {
        payload[i] = i;
    }

    bm_aead_command_t command = {
        .decrypt         = false,
        .key             = key,
        .nonce           = nonce,
        .payload         = payload,
        .payload_size    = 128,
        .additional_data = ad,
        .ad_size         = 16,
        .data_out        = data_out,
        .tag             = tag,
    };

    bool ok = test_aead_command(aead, &command);
    printf("Encryption test %s.\n", ok ? "passed" : "failed");

    command.decrypt  = true;
    command.payload  = data_out;
    command.data_out = data_out_dec;

    ok = test_aead_command(aead, &command);
    printf("Decryption test %s.\n", ok ? "passed" : "failed");

    ok = true;
    for (unsigned i = 0; i < command.payload_size; ++i)
    {
        if (payload[i] != data_out_dec[i])
        {
            ok = false;
            printf("Data missmatch @%u Expected: 0x%x Got: 0x%x\n", i, payload[i], data_out_dec[i]);
        }
    }

    printf("Decrypted data %s the original data.\n", ok ? "matches" : "differs from");

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

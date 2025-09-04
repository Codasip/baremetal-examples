/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/id_reg.h"

#include <baremetal/common.h>
#include <baremetal/platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    puts("Welcome to the ID REGISTERS demo!\n");

    bm_id_reg_t *id_reg = (bm_id_reg_t *)target_peripheral_get(BM_PERIPHERAL_ID_REG);

    if (id_reg == NULL)
    {
        puts("Error: Failed to get ID_REG peripheral");
        return EXIT_FAILURE;
    }

    printf("ID registers information:\n");

    printf("\n");

    printf(" - Platform ID Version:       %u.%u\n",
           bm_id_reg_id_version_get_major_version(id_reg),
           bm_id_reg_id_version_get_minor_version(id_reg));

    printf(" - Platform Version:          %u.%u\n",
           bm_id_reg_plat_version_get_major_version(id_reg),
           bm_id_reg_plat_version_get_minor_version(id_reg));

    bm_id_board_t board = bm_id_reg_plat_version_get_board_id(id_reg);
    printf(" - Board:                     0x%02x (%s)\n",
           board,
           board == BM_ID_BOARD_GENESYS2 ? "Genesys2"
           : board == BM_ID_BOARD_VU19P  ? "VU19P"
           : board == BM_ID_BOARD_VCU118 ? "VCU118"
                                         : "Unknown board");

    bm_id_core_type_t core_type = bm_id_reg_core_type_get_val(id_reg);
    printf(" - Core type:                 0x%02x (%s)\n",
           core_type,
           core_type == BM_ID_CORE_A730   ? "A730"
           : core_type == BM_ID_CORE_L110 ? "L110"
           : core_type == BM_ID_CORE_L730 ? "L730"
           : core_type == BM_ID_CORE_L31  ? "L31"
           : core_type == BM_ID_CORE_X730 ? "X730"
           : core_type == BM_ID_CORE_L150 ? "L150"
           : core_type == BM_ID_CORE_V730 ? "V730"
                                          : "Unknown Core type");

    printf(" - Core Frequency:            %" PRIu32 " MHz\n", bm_id_reg_core_f_get_val(id_reg));

    bm_id_eth_type_t eth_type = bm_id_reg_eth_type_get_val(id_reg);
    printf(" - Ethernet Interface:        0x%02x (%s)\n",
           eth_type,
           eth_type == BM_ID_ETH_LITE      ? "Ethernet Lite"
           : eth_type == BM_ID_ETH_GIGABIT ? "Gigabit Ethernet"
                                           : "Unknown Ethernet Interface");

    printf("\n");

    printf(" - Features:\n");
    printf("    - CHERI:                  %s\n",
           bm_id_reg_features_get_cheri_enabled(id_reg) ? "enabled" : "disabled");

    printf("\n");

    printf(" - Security:\n");
    printf("    - Enabled:                %s\n",
           bm_id_reg_security_get_securiry_enabled(id_reg) ? "true" : "false");

    printf("\n");

    // Get the bitstream build epoch as a UNIX timestamp
    time_t epoch_val = (time_t)bm_id_reg_epoch_get_val(id_reg);

    // Convert the UNIX timestamp to a local time struct
    const struct tm *epoch_tm = localtime(&epoch_val);

    // Format the time into a human-readable string
    char epoch_val_str[64];
    strftime(epoch_val_str, sizeof(epoch_val_str), "%Y-%m-%d %H:%M:%S", epoch_tm);

    printf(" - Bitstream build date:      %s\n", epoch_val_str);

    printf("\n");

    printf(" - Core Harts:                %" PRIu32 "\n", bm_id_reg_harts_get_val(id_reg));

    printf("\n");

    uint32_t uuid[4] = {
        bm_id_reg_core_artifact_id_get_val_0(id_reg),
        bm_id_reg_core_artifact_id_get_val_1(id_reg),
        bm_id_reg_core_artifact_id_get_val_2(id_reg),
        bm_id_reg_core_artifact_id_get_val_3(id_reg),
    };
    char uuid_bytestr[2 * sizeof(uuid) + 1] = {0};
    for (size_t i = 0; i < sizeof(uuid); i++)
    {
        sprintf(&uuid_bytestr[2 * i], "%02x", ((uint8_t *)uuid)[i]);
    }
    printf(" - Core Artifactory ID:       %s\n", uuid_bytestr);

    uint32_t git_id[5] = {
        bm_id_reg_plat_hash_get_val_0(id_reg),
        bm_id_reg_plat_hash_get_val_1(id_reg),
        bm_id_reg_plat_hash_get_val_2(id_reg),
        bm_id_reg_plat_hash_get_val_3(id_reg),
        bm_id_reg_plat_hash_get_val_4(id_reg),
    };
    char git_id_bytestr[2 * sizeof(git_id) + 1] = {0};
    for (size_t i = 0; i < sizeof(git_id); i++)
    {
        sprintf(&git_id_bytestr[2 * i], "%02x", ((uint8_t *)git_id)[i]);
    }
    printf(" - Platform Git Commit ID:    %s\n", git_id_bytestr);

    printf("\n");

    return EXIT_SUCCESS;
}

/* Copyright 2025-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/id_reg.h"

// ID VERSION register MACROs
#define ID_VERSION_MINOR_VERSION_MASK         (0xFF)
#define ID_VERSION_MAJOR_VERSION_MASK         (0xFF << 8)

#define ID_VERSION_GET_MINOR_VERSION(reg_val) ((reg_val)&ID_VERSION_MINOR_VERSION_MASK)
#define ID_VERSION_GET_MAJOR_VERSION(reg_val) (((reg_val)&ID_VERSION_MAJOR_VERSION_MASK) >> 8)

// PLAT VERSION register MACROs
#define PLAT_VERSION_MINOR_VERSION_MASK         (0xFF)
#define PLAT_VERSION_MAJOR_VERSION_MASK         (0xFF << 8)
#define PLAT_VERSION_BOARD_ID_MASK              (0xFF << 16)

#define PLAT_VERSION_GET_MINOR_VERSION(reg_val) ((reg_val)&PLAT_VERSION_MINOR_VERSION_MASK)
#define PLAT_VERSION_GET_MAJOR_VERSION(reg_val) (((reg_val)&PLAT_VERSION_MAJOR_VERSION_MASK) >> 8)
#define PLAT_VERSION_GET_BOARD_ID(reg_val)      (((reg_val)&PLAT_VERSION_BOARD_ID_MASK) >> 16)

// FEATURES register MACROs
#define FEATURES_BIT_CHERI (1 << 0) // Cheri enabled, 1 - Enabled, 0 - Disabled

// SECURITY register MACROs
#define SECURITY_BIT_SECURITY (1 << 0) // Security enabled, 1 - Enabled, 0 - Disabled

// PLAT_INFO register MACROs
#define PLAT_INFO_STR_OFFSET1_MASK         (0xFFFF)
#define PLAT_INFO_STR_OFFSET2_MASK         (0xFFFF << 16)

#define PLAT_INFO_GET_STR_OFFSET1(reg_val) ((reg_val)&PLAT_INFO_STR_OFFSET1_MASK)
#define PLAT_INFO_GET_STR_OFFSET2(reg_val) (((reg_val)&PLAT_INFO_STR_OFFSET2_MASK) >> 16)

uint8_t bm_id_reg_id_version_get_minor_version(bm_id_reg_t *id)
{
    return ID_VERSION_GET_MINOR_VERSION(id->regs->ID_VERSION);
}

uint8_t bm_id_reg_id_version_get_major_version(bm_id_reg_t *id)
{
    return ID_VERSION_GET_MAJOR_VERSION(id->regs->ID_VERSION);
}

uint8_t bm_id_reg_plat_version_get_minor_version(bm_id_reg_t *id)
{
    return PLAT_VERSION_GET_MINOR_VERSION(id->regs->PLAT_VERSION);
}

uint8_t bm_id_reg_plat_version_get_major_version(bm_id_reg_t *id)
{
    return PLAT_VERSION_GET_MAJOR_VERSION(id->regs->PLAT_VERSION);
}

bm_id_board_t bm_id_reg_plat_version_get_board_id(bm_id_reg_t *id)
{
    return PLAT_VERSION_GET_BOARD_ID(id->regs->PLAT_VERSION);
}

bm_id_core_type_t bm_id_reg_core_type_get_val(bm_id_reg_t *id)
{
    return id->regs->CORE_TYPE;
}

uint32_t bm_id_reg_core_f_get_val(bm_id_reg_t *id)
{
    return id->regs->CORE_F;
}

bm_id_eth_type_t bm_id_reg_eth_type_get_val(bm_id_reg_t *id)
{
    return id->regs->ETH_TYPE;
}

bool bm_id_reg_features_get_cheri_enabled(bm_id_reg_t *id)
{
    return (id->regs->FEATURES & FEATURES_BIT_CHERI);
}

bool bm_id_reg_security_get_securiry_enabled(bm_id_reg_t *id)
{
    return (id->regs->SECURITY & SECURITY_BIT_SECURITY);
}

uint32_t bm_id_reg_epoch_get_val(bm_id_reg_t *id)
{
    return id->regs->EPOCH;
}

uint32_t bm_id_reg_harts_get_val(bm_id_reg_t *id)
{
    return id->regs->HARTS;
}

uint16_t bm_id_reg_plat_info_get_str_offset1(bm_id_reg_t *id)
{
    return PLAT_INFO_GET_STR_OFFSET1(id->regs->PLAT_INFO);
}

uint16_t bm_id_reg_plat_info_get_str_offset2(bm_id_reg_t *id)
{
    return PLAT_INFO_GET_STR_OFFSET2(id->regs->PLAT_INFO);
}

uint32_t bm_id_reg_plat_hash_get_val_0(bm_id_reg_t *id)
{
    return id->regs->PLAT_SHA0;
}

uint32_t bm_id_reg_plat_hash_get_val_1(bm_id_reg_t *id)
{
    return id->regs->PLAT_SHA1;
}

uint32_t bm_id_reg_plat_hash_get_val_2(bm_id_reg_t *id)
{
    return id->regs->PLAT_SHA2;
}

uint32_t bm_id_reg_plat_hash_get_val_3(bm_id_reg_t *id)
{
    return id->regs->PLAT_SHA3;
}

uint32_t bm_id_reg_plat_hash_get_val_4(bm_id_reg_t *id)
{
    return id->regs->PLAT_SHA4;
}

uint32_t bm_id_reg_core_artifact_id_get_val_0(bm_id_reg_t *id)
{
    return id->regs->CORE_ARTIFACT0;
}

uint32_t bm_id_reg_core_artifact_id_get_val_1(bm_id_reg_t *id)
{
    return id->regs->CORE_ARTIFACT1;
}

uint32_t bm_id_reg_core_artifact_id_get_val_2(bm_id_reg_t *id)
{
    return id->regs->CORE_ARTIFACT2;
}

uint32_t bm_id_reg_core_artifact_id_get_val_3(bm_id_reg_t *id)
{
    return id->regs->CORE_ARTIFACT3;
}

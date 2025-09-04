/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/id_reg.h"

/** \brief Platform ID registers */
struct bm_id_regs {
    const volatile uint32_t ID_VERSION;   ///< (0x0000) Platform ID register version
    const volatile uint32_t PLAT_VERSION; ///< (0x0004) Platform version
    const volatile uint32_t CORE_TYPE;    ///< (0x0008) Core type
    const volatile uint32_t CORE_F;       ///< (0x000C) Core frequency in MHz
    const volatile uint32_t ETH_TYPE;     ///< (0x0010) Ethernet type
    const volatile uint32_t FEATURES;     ///< (0x0014) Platform features
    const volatile uint32_t SECURITY;     ///< (0x0018) Platform security features
    uint8_t                 _reserved1[0x04];
    const volatile uint32_t EPOCH;            ///< (0x0020) Epoch
    const volatile uint32_t HARTS;            ///< (0x0024) Core Harts
    const volatile uint32_t PLAT_INFO;        ///< (0x0028) Platform Information
    uint8_t                 _reserved2[0xD4]; ///< (0x002c - 0x0100) reserved
    const volatile uint32_t CORE_ARTIFACT0; ///< (0x0100) ARTIFACT byte array (artifact[3]:artifact[2]:artifact[1]:artifact[0])
    const volatile uint32_t CORE_ARTIFACT1; ///< (0x0104) ARTIFACT byte array (artifact[7]:artifact[6]:artifact[5]:artifact[4])
    const volatile uint32_t CORE_ARTIFACT2; ///< (0x0108) ARTIFACT byte array (artifact[11]:artifact[10]:artifact[9]:artifact[8])
    const volatile uint32_t CORE_ARTIFACT3; ///< (0x010C) ARTIFACT byte array (artifact[15]:artifact[14]:artifact[13]:artifact[12])
    const volatile uint32_t PLAT_SHA0; ///< (0x0110) SHA byte array (sha[3]:sha[2]:sha[1]:sha[0])
    const volatile uint32_t PLAT_SHA1; ///< (0x0114) SHA byte array (sha[7]:sha[6]:sha[5]:sha[4])
    const volatile uint32_t PLAT_SHA2; ///< (0x0118) SHA byte array (sha[11]:sha[10]:sha[9]:sha[8])
    const volatile uint32_t PLAT_SHA3; ///< (0x011C) SHA byte array (sha[15]:sha[14]:sha[13]:sha[12])
    const volatile uint32_t PLAT_SHA4; ///< (0x0120) SHA byte array (sha[19]:sha[18]:sha[17]:sha[16])
};

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

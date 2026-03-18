/* Copyright 2024-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_IDREG_H
#define BAREMETAL_IDREG_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/** \brief Structure describing Platform ID registers */
typedef struct bm_id_regs bm_id_regs_t;

typedef struct {
    bm_id_regs_t *regs; ///< Pointer to the peripheral registers
} bm_id_reg_t;

typedef enum {
    BM_ID_BOARD_GENESYS2 = 0x01,
    BM_ID_BOARD_VU19P    = 0x02,
    BM_ID_BOARD_VCU118   = 0x03,
    BM_ID_BOARD_KR260    = 0x04,
} bm_id_board_t;

/** \brief Core type options */
typedef enum {
    BM_ID_CORE_A730 = 0x01,
    BM_ID_CORE_L110 = 0x02,
    BM_ID_CORE_L730 = 0x03,
    BM_ID_CORE_L31  = 0x04,
    BM_ID_CORE_X730 = 0x05,
    BM_ID_CORE_L150 = 0x06,
    BM_ID_CORE_V730 = 0x07,
} bm_id_core_type_t;

/** \brief Ethernet type options */
typedef enum {
    BM_ID_ETH_LITE    = 0x00,
    BM_ID_ETH_GIGABIT = 0x01,
} bm_id_eth_type_t;

/** \brief Feature options */
typedef enum {
    BM_ID_FEATURE_CHERI = 0x01,
} bm_id_feature_t;

typedef enum {
    BM_ID_SECUR_FEATURE = 0x00,
} bm_id_secure_feature_t;

/**
 * \brief Get Minor Version from ID_VERSION register
 *
 * \param id ID registers device
 *
 * \return ID Version Minor
 */
uint8_t bm_id_reg_id_version_get_minor_version(bm_id_reg_t *id);

/**
 * \brief Get Major Version from ID_VERSION register
 *
 * \param id ID registers device
 *
 * \return ID Version Major
 */
uint8_t bm_id_reg_id_version_get_major_version(bm_id_reg_t *id);

/**
 * \brief Get Minor Version from PLAT_VERSION register
 *
 * \param id ID registers device
 *
 * \return PLAT Version Minor
 */
uint8_t bm_id_reg_plat_version_get_minor_version(bm_id_reg_t *id);

/**
 * \brief Get Major Version from PLAT_VERSION register
 *
 * \param id ID registers device
 *
 * \return PLAT Version Major
 */
uint8_t bm_id_reg_plat_version_get_major_version(bm_id_reg_t *id);

/**
 * \brief Get Board ID from PLAT_VERSION register
 *
 * \param id ID registers device
 *
 * \return BOARD ID. \ref bm_id_board_t
 */
bm_id_board_t bm_id_reg_plat_version_get_board_id(bm_id_reg_t *id);

/**
 * \brief Get Core Type from CORE_TYPE register
 *
 * \param id ID registers device
 *
 * \return Core type. \ref bm_id_core_type_t
 */
bm_id_core_type_t bm_id_reg_core_type_get_val(bm_id_reg_t *id);

/**
 * \brief Get Core Frequency from CORE_F register
 *
 * \param id ID registers device
 *
 * \return Core Frequency in MHz
 */
uint32_t bm_id_reg_core_f_get_val(bm_id_reg_t *id);

/**
 * \brief Get Ethernet Type from ETH_TYPE register
 *
 * \param id ID registers device
 *
 * \return Ethernet type. \ref bm_id_eth_type_t
 */
bm_id_eth_type_t bm_id_reg_eth_type_get_val(bm_id_reg_t *id);

/**
 * \brief Get Cheri Enabled Bit from FEATURES register
 *
 * \param id ID registers device
 *
 * \return true/false
 */
bool bm_id_reg_features_get_cheri_enabled(bm_id_reg_t *id);

/**
 * \brief Get Security Enabled Bit from SECURITY register
 *
 * \param id ID registers device
 *
 * \return true/false
 */
bool bm_id_reg_security_get_securiry_enabled(bm_id_reg_t *id);

/**
 * \brief Get Bitstream build timestamp from EPOCH register
 *
 * \param id ID registers device
 *
 * \return Bitstream build timestamp (UNIX time)
 */
uint32_t bm_id_reg_epoch_get_val(bm_id_reg_t *id);

/**
 * \brief Get Core Harts from HARTS register
 *
 * \param id ID registers device
 *
 * \return Core Harts val
 */
uint32_t bm_id_reg_harts_get_val(bm_id_reg_t *id);

/**
 * \brief Get STR_OFFSET1 from PLAT_INFO register
 *  This is an offset to information string
 *
 * \param id ID registers device
 *
 * \return Offset value
 */
uint16_t bm_id_reg_plat_info_get_str_offset1(bm_id_reg_t *id);

/**
 * \brief Get STR_OFFSET2 from PLAT_INFO register
 *  This is an offset to information string
 *
 * \param id ID registers device
 *
 * \return Offset value
 */
uint16_t bm_id_reg_plat_info_get_str_offset2(bm_id_reg_t *id);

/**
 * \brief Get PLAT_SHA0 register val
 *
 * \param id ID registers device
 *
 * \return SHA byte array (sha[3]:sha[2]:sha[1]:sha[0])
 */
uint32_t bm_id_reg_plat_hash_get_val_0(bm_id_reg_t *id);

/**
 * \brief Get PLAT_SHA1 register val
 *
 * \param id ID registers device
 *
 * \return SHA byte array (sha[7]:sha[6]:sha[5]:sha[4])
 */
uint32_t bm_id_reg_plat_hash_get_val_1(bm_id_reg_t *id);

/**
 * \brief Get PLAT_SHA2 register val
 *
 * \param id ID registers device
 *
 * \return SHA byte array (sha[11]:sha[10]:sha[9]:sha[8])
 */
uint32_t bm_id_reg_plat_hash_get_val_2(bm_id_reg_t *id);

/**
 * \brief Get PLAT_SHA3 register val
 *
 * \param id ID registers device
 *
 * \return SHA byte array (sha[15]:sha[14]:sha[13]:sha[12])
 */
uint32_t bm_id_reg_plat_hash_get_val_3(bm_id_reg_t *id);

/**
 * \brief Get PLAT_SHA4 register val
 *
 * \param id ID registers device
 *
 * \return SHA byte array (sha[19]:sha[18]:sha[17]:sha[16])
 */
uint32_t bm_id_reg_plat_hash_get_val_4(bm_id_reg_t *id);

/**
 * \brief Get CORE_ARTIFACT0 register val
 *
 * \param id ID registers device
 *
 * \return ARTIFACT byte array (artifact[3]:artifact[2]:artifact[1]:artifact[0])
 */
uint32_t bm_id_reg_core_artifact_id_get_val_0(bm_id_reg_t *id);

/**
 * \brief Get CORE_ARTIFACT1 register val
 *
 * \param id ID registers device
 *
 * \return ARTIFACT byte array (artifact[7]:artifact[6]:artifact[5]:artifact[4])
 */
uint32_t bm_id_reg_core_artifact_id_get_val_1(bm_id_reg_t *id);

/**
 * \brief Get CORE_ARTIFACT2 register val
 *
 * \param id ID registers device
 *
 * \return ARTIFACT byte array (artifact[11]:artifact[10]:artifact[9]:artifact[8])
 */
uint32_t bm_id_reg_core_artifact_id_get_val_2(bm_id_reg_t *id);

/**
 * \brief Get CORE_ARTIFACT3 register val
 *
 * \param id ID registers device
 *
 * \return ARTIFACT byte array (artifact[15]:artifact[14]:artifact[13]:artifact[12])
 */
uint32_t bm_id_reg_core_artifact_id_get_val_3(bm_id_reg_t *id);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_IDREG_H */

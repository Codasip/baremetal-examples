/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_IDREG_H
#define BAREMETAL_IDREG_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Platform ID registers */
typedef struct bm_id_regs {
    const volatile uint32_t VERSION;   ///< (0x0000) Platform ID register version
    const volatile uint32_t PLAT_VER;  ///< (0x0004) Platform version
    const volatile uint32_t CORE_TYPE; ///< (0x0008) Core type
    const volatile uint32_t CORE_FREQ; ///< (0x000C) Core frequency
    const volatile uint32_t ETH_TYPE;  ///< (0x0010) Ethernet type
    const volatile uint32_t FEATURES;  ///< (0x0014) Platform features
} bm_id_regs_t;

typedef struct {
    bm_id_regs_t *regs; ///< Pointer to the peripheral registers
} bm_id_reg_t;

/** \brief ID retrievable fields */
typedef enum {
    BM_ID_ID_VERSION_MINOR   = 0x0,
    BM_ID_ID_VERSION_MAJOR   = 0x1,
    BM_ID_PLAT_VERSION_MINOR = 0x2,
    BM_ID_PLAT_VERSION_MAJOR = 0x3,
    BM_ID_CORE_TYPE          = 0x4,
    BM_ID_CORE_FREQ          = 0x5,
    BM_ID_ETH_TYPE           = 0x6,
    BM_ID_FEATURES           = 0x7,
} bm_id_info_t;

/** \brief Core type options */
typedef enum {
    BM_ID_CORE_A730 = 0x1,
} bm_id_core_type_t;

/** \brief Ethernet type options */
typedef enum {
    BM_ID_ETH_LITE    = 0x0,
    BM_ID_ETH_GIGABIT = 0x1,
} bm_id_eth_type_t;

/** \brief feature options */
typedef enum {
    BM_ID_FEATURE_CHERI = 0x1,
} bm_id_feature_t;

/**
 * \brief Get info from platform ID registers
 *
 * \param id ID registers device
 * \param info Info to retrieve
 *
 * \returns -1 on error or requested information
 */
static inline int bm_id_get_info(bm_id_reg_t *id, bm_id_info_t info)
{
    switch (info)
    {
        case BM_ID_ID_VERSION_MINOR:
            return id->regs->VERSION & 0xff;
        case BM_ID_ID_VERSION_MAJOR:
            return (id->regs->VERSION >> 8) & 0xff;
        case BM_ID_PLAT_VERSION_MINOR:
            return id->regs->PLAT_VER & 0xff;
        case BM_ID_PLAT_VERSION_MAJOR:
            return (id->regs->PLAT_VER >> 8) & 0xff;
        case BM_ID_CORE_TYPE:
            return id->regs->CORE_TYPE;
        case BM_ID_CORE_FREQ:
            return id->regs->CORE_FREQ;
        case BM_ID_ETH_TYPE:
            return id->regs->ETH_TYPE;
        case BM_ID_FEATURES:
            return id->regs->FEATURES;
        default:
            return -1;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_IDREG_H */

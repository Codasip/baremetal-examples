/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_PMP_H
#define BAREMETAL_PMP_H

#include "baremetal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief PMP addressing modes */
typedef enum {
    BM_PMP_MODE_OFF   = 0x0,
    BM_PMP_MODE_TOR   = 0x8,
    BM_PMP_MODE_NA4   = 0x10,
    BM_PMP_MODE_NAPOT = 0x18,
} bm_pmp_address_mode_t;

/** \brief PMP entry configuration options */
typedef enum {
    BM_PMP_CONFIG_READ  = 0x1,
    BM_PMP_MODE_WRITE   = 0x2,
    BM_PMP_MODE_EXECUTE = 0x4,
    BM_PMP_MODE_LOCK    = 0x80,
} bm_pmp_config_t;

/**
 * \brief Setup a PMP entry
 *
 * \param index PMP entry to setup
 * \param address Value specifying addresses to match (meaning depends on adress matching mode)
 * \param address_mode Mode of address matching
 * \param config PMP configuration to apply on the configured address range (logical OR of bm_pmp_config_t)
 */
void bm_pmp_set(unsigned index, xlen_t address, bm_pmp_address_mode_t address_mode, unsigned config);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_PMP_H */

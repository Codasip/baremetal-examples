/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_TARGET_CACHE_H
#define BAREMETAL_TARGET_CACHE_H

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/verbose.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BM_MDCACHECTRL_CLEAN            0x1
#define BM_MDCACHECTRL_FLUSH            0x3
#define BM_MDCACHECTRL_DISABLE          0x4
#define BM_MDCACHECTRL_ENABLE           0x5
#define BM_MDCACHECTRL_DISABLE_PREFETCH 0x6
#define BM_MDCACHECTRL_ENABLE_PREFETCH  0x7

#define BM_MICACHECTRL_DISABLE          0x1
#define BM_MICACHECTRL_ENABLE           0x2
#define BM_MICACHECTRL_DISABLE_PREFETCH 0x3
#define BM_MICACHECTRL_ENABLE_PREFETCH  0x4
#define BM_MICACHECTRL_INVALIDATE       0x5

#define BM_ML2CACHECTRL_CLEAN           0x1
#define BM_ML2CACHECTRL_FLUSH           0x3
#define BM_ML2CACHECTRL_DISABLE         0x4
#define BM_ML2CACHECTRL_ENABLE          0x5
#define BM_ML2CACHECTRL_DISABLE_ACP     0x6
#define BM_ML2CACHECTRL_ENABLE_ACP      0x7
#define BM_ML2CACHECTRL_DISABLE_SCU     0x8
#define BM_ML2CACHECTRL_ENABLE_SCU      0x9

/**
 * \brief Flush given address in the data cache
 *
 * \param addr Address to flush
 */
static inline void bm_dcache_flush_address(xlen_t addr)
{
    __asm__ volatile("cbo.clean (%0)" ::"r"(addr));
}

/**
 * \brief Invalidate given address in the data cache
 *
 * \param addr Address to invalidate
 */
static inline void bm_dcache_invalidate_address(xlen_t addr)
{
    // clean + invalidate
    __asm__ volatile("cbo.flush (%0)" ::"r"(addr));
}

/**
 * \brief Flush all dirty lines in the data cache
 */
static inline void bm_dcache_flush_all(void)
{
    bm_csr_write(BM_CSR_MDCACHECTRL, BM_MDCACHECTRL_CLEAN);
    bm_csr_write(BM_CSR_ML2CACHECTRL, BM_ML2CACHECTRL_CLEAN);
}

/**
 * \brief Invalidate all lines in the data cache
 */
static inline void bm_dcache_invalidate_all(void)
{
    // clean + invalidate
    bm_csr_write(BM_CSR_MDCACHECTRL, BM_MDCACHECTRL_FLUSH);
    bm_csr_write(BM_CSR_ML2CACHECTRL, BM_ML2CACHECTRL_FLUSH);
}

/**
 * \brief Invalidate all lines in the instruction cache
 */
static inline void bm_icache_invalidate_all(void)
{
    bm_csr_write(BM_CSR_MICACHECTRL, BM_MICACHECTRL_INVALIDATE);
}

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_TARGET_CACHE_H */

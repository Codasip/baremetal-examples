/* Copyright 2023 Codasip s.r.o.         */
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

/** \brief Cache control and status registers */
typedef struct {
    volatile xlen_t TYPE;    ///< Type of the cache
    volatile xlen_t VERSION; ///< Version of the cache
    volatile xlen_t ISIZE;   ///< Size of the instruction cache
    volatile xlen_t IASSOC;  ///< Number of ways of the instruction cache
    volatile xlen_t ILINE;   ///< Cache line size of the instruction cache
    volatile xlen_t DSIZE;   ///< Size of the data cache
    volatile xlen_t DASSOC;  ///< Number of ways of the data cache
    volatile xlen_t DLINE;   ///< Cache line size of the data cache
    xlen_t          _reserved1[24];
    volatile xlen_t IHIT;  ///< Number of instruction cache hits
    volatile xlen_t IMISS; ///< Number of instruction cache misses
    volatile xlen_t DHIT;  ///< Number of data cache hits
    volatile xlen_t DMISS; ///< Number of data cache misses
    volatile xlen_t IERR;  ///< Number of errors on instruction cache interface
    volatile xlen_t DERR;  ///< Number of errors on data cache interface
    xlen_t          _reserved2[10];
    volatile xlen_t DWT; ///< Data cache write-through settings
    xlen_t          _reserved3[31];
    volatile xlen_t IINVADDR; ///< Instruction cache invalidate address
    volatile xlen_t IINVLN;   ///< Instruction cache invalidate cache line
    volatile xlen_t IINVALL;  ///< Instruction cache invalidate all
    volatile xlen_t DINVADDR; ///< Data cache invalidate address
    volatile xlen_t DINVLN;   ///< Data cache invalidate cache line
    volatile xlen_t DINVALL;  ///< Data cache invalidate all
    volatile xlen_t DFLADDR;  ///< Data cache flush address
    volatile xlen_t DFLLN;    ///< Data cache flush cache line
    volatile xlen_t DFLALL;   ///< Data cache flush all
    xlen_t          _reserved4[23];
    volatile xlen_t CLR; ///< Clear counters
    xlen_t          _reserved5[15];
} CACHE_Type;

/** \brief Enumeration of cache configuration information and performance counters */
typedef enum {
    BM_CACHE_TYPE,
    BM_CACHE_VERSION,
    BM_ICACHE_SIZE,
    BM_ICACHE_NUM_WAYS,
    BM_ICACHE_LINE_SIZE,
    BM_DCACHE_SIZE,
    BM_DCACHE_NUM_WAYS,
    BM_DCACHE_LINE_SIZE,
    BM_ICACHE_HITS,
    BM_ICACHE_MISS,
    BM_DCACHE_HITS,
    BM_DCACHE_MISS,
    BM_ICACHE_ERRORS,
    BM_DCACHE_ERRORS,
} bm_cache_info_t;

/** \brief Enumeration of cache types */
typedef enum {
    BM_CACHE_TYPE_DCACHE = 0x1,
    BM_CACHE_TYPE_ICACHE = 0x2,
} bm_cache_type_t;

/**
 * \brief Get cache base address
 *
 * \return Pointer to the cache registers
 */
static inline CACHE_Type *bm_cache_get_regs(void)
{
    return (CACHE_Type *)bm_csr_read(BM_CSR_ML1CACHE_START);
}

/**
 * \brief Get cache configuration information or a performance counter value
 *
 * \param info Type of information/performance counter to retrieve
 *
 * \return Information retrieved from the specified cache controller register
 */
static inline xlen_t bm_cache_get_info(bm_cache_info_t info)
{
    switch (info)
    {
        case BM_CACHE_TYPE:
            return bm_cache_get_regs()->TYPE;
        case BM_CACHE_VERSION:
            return bm_cache_get_regs()->VERSION;
        case BM_ICACHE_SIZE:
            return bm_cache_get_regs()->ISIZE;
        case BM_ICACHE_NUM_WAYS:
            return bm_cache_get_regs()->IASSOC;
        case BM_ICACHE_LINE_SIZE:
            return bm_cache_get_regs()->ILINE;
        case BM_DCACHE_SIZE:
            return bm_cache_get_regs()->DSIZE;
        case BM_DCACHE_NUM_WAYS:
            return bm_cache_get_regs()->DASSOC;
        case BM_DCACHE_LINE_SIZE:
            return bm_cache_get_regs()->DLINE;
        case BM_ICACHE_HITS:
            return bm_cache_get_regs()->IHIT;
        case BM_ICACHE_MISS:
            return bm_cache_get_regs()->IMISS;
        case BM_DCACHE_HITS:
            return bm_cache_get_regs()->DHIT;
        case BM_DCACHE_MISS:
            return bm_cache_get_regs()->DMISS;
        case BM_ICACHE_ERRORS:
            return bm_cache_get_regs()->IERR;
        case BM_DCACHE_ERRORS:
            return bm_cache_get_regs()->DERR;
    }
    bm_error("Unsupporter cache configuration field.");
}

/**
 * \brief Setup write-through for data cache
 *
 * \param enable Flag whether to enable or disable write-through
 */
static inline void bm_dcache_set_writethrough(bool enable)
{
    bm_cache_get_regs()->DWT = (unsigned)enable;
}

/**
 * \brief Invalidate given address in the instruction cache
 *
 * \param addr Address to invalidate
 */
static inline void bm_icache_invalidate_address(xlen_t addr)
{
    bm_cache_get_regs()->IINVADDR = addr;
}

/**
 * \brief Invalidate given line in the instruction cache
 *
 * \param line Line to invalidate
 */
static inline void bm_icache_invalidate_line(unsigned line)
{
    bm_cache_get_regs()->IINVLN = line;
}

/**
 * \brief Invalidate all lines in the instruction cache
 */
static inline void bm_icache_invalidate_all(void)
{
    bm_cache_get_regs()->IINVALL = 1;
}

/**
 * \brief Flush given address in the data cache
 *
 * \param addr Address to flush
 */
static inline void bm_dcache_flush_address(xlen_t addr)
{
    bm_cache_get_regs()->DFLADDR = addr;
}

/**
 * \brief Flush given line in the data cache
 *
 * \param line Line to flush
 */
static inline void bm_dcache_flush_line(unsigned line)
{
    bm_cache_get_regs()->DFLLN = line;
}

/**
 * \brief Flush all dirty lines in the data cache
 */
static inline void bm_dcache_flush_all(void)
{
    bm_cache_get_regs()->DFLALL = 1;
}

/**
 * \brief Invalidate given address in the data cache
 *
 * \param addr Address to invalidate
 */
static inline void bm_dcache_invalidate_address(xlen_t addr)
{
    // Flush the address first to prevent memory corruption
    bm_dcache_flush_address(addr);
    bm_cache_get_regs()->DINVADDR = addr;
}

/**
 * \brief Invalidate given line in the data cache
 *
 * \param line Line to invalidate
 */
static inline void bm_dcache_invalidate_line(unsigned line)
{
    // Flush the line first to prevent memory corruption
    bm_dcache_flush_line(line);
    bm_cache_get_regs()->DINVLN = line;
}

/**
 * \brief Invalidate all lines in the data cache
 */
static inline void bm_dcache_invalidate_all(void)
{
    // Flush the cache first to prevent memory corruption
    bm_dcache_flush_all();
    bm_cache_get_regs()->DINVALL = 1;
}

/**
 * \brief Clear all performance counters
 */
static inline void bm_cache_clear_counters(void)
{
    bm_cache_get_regs()->CLR = 1;
}

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_TARGET_CACHE_H */

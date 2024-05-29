/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/counter.h"

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/priv.h"
#include "baremetal/verbose.h"

#include <stdint.h>

/** \brief Helper table with CSR IDs for machine mode counters */
const int COUNTER_CSR_M_TABLE[] = {
    BM_CSR_MCYCLE,
    BM_CSR_UNDEFINED, // not available
    BM_CSR_MINSTRET,      BM_CSR_MHPMCOUNTER3,  BM_CSR_MHPMCOUNTER4,  BM_CSR_MHPMCOUNTER5,
    BM_CSR_MHPMCOUNTER6,  BM_CSR_MHPMCOUNTER7,  BM_CSR_MHPMCOUNTER8,  BM_CSR_MHPMCOUNTER9,
    BM_CSR_MHPMCOUNTER10, BM_CSR_MHPMCOUNTER11, BM_CSR_MHPMCOUNTER12, BM_CSR_MHPMCOUNTER13,
    BM_CSR_MHPMCOUNTER14, BM_CSR_MHPMCOUNTER15, BM_CSR_MHPMCOUNTER16, BM_CSR_MHPMCOUNTER17,
    BM_CSR_MHPMCOUNTER18, BM_CSR_MHPMCOUNTER19, BM_CSR_MHPMCOUNTER20, BM_CSR_MHPMCOUNTER21,
    BM_CSR_MHPMCOUNTER22, BM_CSR_MHPMCOUNTER23, BM_CSR_MHPMCOUNTER24, BM_CSR_MHPMCOUNTER25,
    BM_CSR_MHPMCOUNTER26, BM_CSR_MHPMCOUNTER27, BM_CSR_MHPMCOUNTER28, BM_CSR_MHPMCOUNTER29,
    BM_CSR_MHPMCOUNTER30, BM_CSR_MHPMCOUNTER31,
};

/** \brief Helper table with CSR IDs for user/supervisor mode counters */
const int COUNTER_CSR_TABLE[] = {
    BM_CSR_CYCLE,        BM_CSR_TIME,         BM_CSR_INSTRET,      BM_CSR_HPMCOUNTER3,
    BM_CSR_HPMCOUNTER4,  BM_CSR_HPMCOUNTER5,  BM_CSR_HPMCOUNTER6,  BM_CSR_HPMCOUNTER7,
    BM_CSR_HPMCOUNTER8,  BM_CSR_HPMCOUNTER9,  BM_CSR_HPMCOUNTER10, BM_CSR_HPMCOUNTER11,
    BM_CSR_HPMCOUNTER12, BM_CSR_HPMCOUNTER13, BM_CSR_HPMCOUNTER14, BM_CSR_HPMCOUNTER15,
    BM_CSR_HPMCOUNTER16, BM_CSR_HPMCOUNTER17, BM_CSR_HPMCOUNTER18, BM_CSR_HPMCOUNTER19,
    BM_CSR_HPMCOUNTER20, BM_CSR_HPMCOUNTER21, BM_CSR_HPMCOUNTER22, BM_CSR_HPMCOUNTER23,
    BM_CSR_HPMCOUNTER24, BM_CSR_HPMCOUNTER25, BM_CSR_HPMCOUNTER26, BM_CSR_HPMCOUNTER27,
    BM_CSR_HPMCOUNTER28, BM_CSR_HPMCOUNTER29, BM_CSR_HPMCOUNTER30, BM_CSR_HPMCOUNTER31,
};

/**
 * \brief Helper function for obtaining CSR ID corresponding to given counter
 */
static inline int bm_counter_get_csr_id(const bm_counter_id counter)
{
    if (bm_get_priv_mode() == BM_PRIV_MODE_MACHINE)
    {
        return COUNTER_CSR_M_TABLE[counter];
    }
    return COUNTER_CSR_TABLE[counter];
}

#if __riscv_xlen == 32
/** \brief Helper table with CSR IDs for higher bits of machine mode counters */
const int COUNTER_CSR_H_M_TABLE[] = {
    BM_CSR_MCYCLEH,
    BM_CSR_UNDEFINED, // not available
    BM_CSR_MINSTRETH,      BM_CSR_MHPMCOUNTER3H,  BM_CSR_MHPMCOUNTER4H,  BM_CSR_MHPMCOUNTER5H,
    BM_CSR_MHPMCOUNTER6H,  BM_CSR_MHPMCOUNTER7H,  BM_CSR_MHPMCOUNTER8H,  BM_CSR_MHPMCOUNTER9H,
    BM_CSR_MHPMCOUNTER10H, BM_CSR_MHPMCOUNTER11H, BM_CSR_MHPMCOUNTER12H, BM_CSR_MHPMCOUNTER13H,
    BM_CSR_MHPMCOUNTER14H, BM_CSR_MHPMCOUNTER15H, BM_CSR_MHPMCOUNTER16H, BM_CSR_MHPMCOUNTER17H,
    BM_CSR_MHPMCOUNTER18H, BM_CSR_MHPMCOUNTER19H, BM_CSR_MHPMCOUNTER20H, BM_CSR_MHPMCOUNTER21H,
    BM_CSR_MHPMCOUNTER22H, BM_CSR_MHPMCOUNTER23H, BM_CSR_MHPMCOUNTER24H, BM_CSR_MHPMCOUNTER25H,
    BM_CSR_MHPMCOUNTER26H, BM_CSR_MHPMCOUNTER27H, BM_CSR_MHPMCOUNTER28H, BM_CSR_MHPMCOUNTER29H,
    BM_CSR_MHPMCOUNTER30H, BM_CSR_MHPMCOUNTER31H,
};

/** \brief Helper table with CSR IDs for higher bits of user/supervisor mode counters */
const int COUNTER_CSR_H_TABLE[] = {
    BM_CSR_CYCLEH,        BM_CSR_TIMEH,         BM_CSR_INSTRETH,      BM_CSR_HPMCOUNTER3H,
    BM_CSR_HPMCOUNTER4H,  BM_CSR_HPMCOUNTER5H,  BM_CSR_HPMCOUNTER6H,  BM_CSR_HPMCOUNTER7H,
    BM_CSR_HPMCOUNTER8H,  BM_CSR_HPMCOUNTER9H,  BM_CSR_HPMCOUNTER10H, BM_CSR_HPMCOUNTER11H,
    BM_CSR_HPMCOUNTER12H, BM_CSR_HPMCOUNTER13H, BM_CSR_HPMCOUNTER14H, BM_CSR_HPMCOUNTER15H,
    BM_CSR_HPMCOUNTER16H, BM_CSR_HPMCOUNTER17H, BM_CSR_HPMCOUNTER18H, BM_CSR_HPMCOUNTER19H,
    BM_CSR_HPMCOUNTER20H, BM_CSR_HPMCOUNTER21H, BM_CSR_HPMCOUNTER22H, BM_CSR_HPMCOUNTER23H,
    BM_CSR_HPMCOUNTER24H, BM_CSR_HPMCOUNTER25H, BM_CSR_HPMCOUNTER26H, BM_CSR_HPMCOUNTER27H,
    BM_CSR_HPMCOUNTER28H, BM_CSR_HPMCOUNTER29H, BM_CSR_HPMCOUNTER30H, BM_CSR_HPMCOUNTER31H,
};

/**
 * \brief Helper function for obtaining CSR ID corresponding to higher bits of given counter
 */
static inline int bm_counter_get_csr_h_id(const bm_counter_id counter)
{
    if (bm_get_priv_mode() == BM_PRIV_MODE_MACHINE)
    {
        return COUNTER_CSR_H_M_TABLE[counter];
    }
    return COUNTER_CSR_H_TABLE[counter];
}
#endif

uint64_t bm_counter_read(const bm_counter_id counter)
{
    int csr = bm_counter_get_csr_id(counter);
#if __riscv_xlen == 32
    // Counters are split in two CSRs for 32-bit targets, the following sequence ensures
    // correct reading even if the counter increments in between the CSR accesses.
    int csrh = bm_counter_get_csr_h_id(counter);

    uint32_t low, high, high_tmp;
    do
    {
        high_tmp = bm_csr_read(csrh);
        low      = bm_csr_read(csr);
        high     = bm_csr_read(csrh);
    } while (high_tmp != high);
    return ((uint64_t)high << 32) | low;
#else
    return bm_csr_read(csr);
#endif
}

void bm_counter_clear(const bm_counter_id counter)
{
    int csr = bm_counter_get_csr_id(counter);
    bm_csr_write(csr, 0);
#if __riscv_xlen == 32
    int csrh = bm_counter_get_csr_h_id(counter);
    bm_csr_write(csrh, 0);
#endif
}

#if defined(TARGET_EXT_U) || defined(TARGET_EXT_S)
void bm_counter_enable_priv(bm_priv_mode_t priv_mode, const bm_counter_id counter)
{
    if (bm_get_priv_mode() <= priv_mode)
    {
        bm_error("Cannot enable for higher mode.");
    }

    xlen_t mask = 1 << counter;

    if (bm_get_priv_mode() == BM_PRIV_MODE_MACHINE)
    {
        bm_csr_set_mask(BM_CSR_MCOUNTEREN, mask);
    }

    #if defined(TARGET_EXT_U) && defined(TARGET_EXT_S)
    if (priv_mode == BM_PRIV_MODE_USER)
    {
        bm_csr_set_mask(BM_CSR_SCOUNTEREN, mask);
    }
    #endif
}
#endif

void bm_counter_pause(const bm_counter_id counter)
{
    if (bm_get_priv_mode() != BM_PRIV_MODE_MACHINE)
    {
        bm_error("Must run in machine mode.");
    }

    xlen_t mask = 1 << counter;
    bm_csr_set_mask(BM_CSR_MCOUNTINHIBIT, mask);
}

void bm_counter_resume(const bm_counter_id counter)
{
    if (bm_get_priv_mode() != BM_PRIV_MODE_MACHINE)
    {
        bm_error("Must run in machine mode.");
    }

    xlen_t mask = 1 << counter;
    bm_csr_clear_mask(BM_CSR_MCOUNTINHIBIT, mask);
}

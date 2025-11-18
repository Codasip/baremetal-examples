/* Copyright 2023-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/counter.h"

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/priv.h"
#include "baremetal/verbose.h"

#include <stdint.h>

/** \brief Helper table with CSR IDs for machine mode counters */
static const int COUNTER_CSR_M_TABLE[] = {
    BM_CSR_MCYCLE,
    BM_CSR_UNDEFINED, // not available
    BM_CSR_MINSTRET,
#ifdef TARGET_HAS_HPM
    BM_CSR_MHPMCOUNTER3,  BM_CSR_MHPMCOUNTER4,  BM_CSR_MHPMCOUNTER5,  BM_CSR_MHPMCOUNTER6,
    BM_CSR_MHPMCOUNTER7,  BM_CSR_MHPMCOUNTER8,  BM_CSR_MHPMCOUNTER9,  BM_CSR_MHPMCOUNTER10,
    BM_CSR_MHPMCOUNTER11, BM_CSR_MHPMCOUNTER12, BM_CSR_MHPMCOUNTER13, BM_CSR_MHPMCOUNTER14,
    BM_CSR_MHPMCOUNTER15, BM_CSR_MHPMCOUNTER16, BM_CSR_MHPMCOUNTER17, BM_CSR_MHPMCOUNTER18,
    BM_CSR_MHPMCOUNTER19, BM_CSR_MHPMCOUNTER20, BM_CSR_MHPMCOUNTER21, BM_CSR_MHPMCOUNTER22,
    BM_CSR_MHPMCOUNTER23, BM_CSR_MHPMCOUNTER24, BM_CSR_MHPMCOUNTER25, BM_CSR_MHPMCOUNTER26,
    BM_CSR_MHPMCOUNTER27, BM_CSR_MHPMCOUNTER28, BM_CSR_MHPMCOUNTER29, BM_CSR_MHPMCOUNTER30,
    BM_CSR_MHPMCOUNTER31,
#endif // TARGET_HAS_HPM
};

/** \brief Helper table with CSR IDs for user/supervisor mode counters */
static const int COUNTER_CSR_TABLE[] = {
    BM_CSR_CYCLE,        BM_CSR_TIME,         BM_CSR_INSTRET,
#ifdef TARGET_HAS_HPM
    BM_CSR_HPMCOUNTER3,  BM_CSR_HPMCOUNTER4,  BM_CSR_HPMCOUNTER5,  BM_CSR_HPMCOUNTER6,
    BM_CSR_HPMCOUNTER7,  BM_CSR_HPMCOUNTER8,  BM_CSR_HPMCOUNTER9,  BM_CSR_HPMCOUNTER10,
    BM_CSR_HPMCOUNTER11, BM_CSR_HPMCOUNTER12, BM_CSR_HPMCOUNTER13, BM_CSR_HPMCOUNTER14,
    BM_CSR_HPMCOUNTER15, BM_CSR_HPMCOUNTER16, BM_CSR_HPMCOUNTER17, BM_CSR_HPMCOUNTER18,
    BM_CSR_HPMCOUNTER19, BM_CSR_HPMCOUNTER20, BM_CSR_HPMCOUNTER21, BM_CSR_HPMCOUNTER22,
    BM_CSR_HPMCOUNTER23, BM_CSR_HPMCOUNTER24, BM_CSR_HPMCOUNTER25, BM_CSR_HPMCOUNTER26,
    BM_CSR_HPMCOUNTER27, BM_CSR_HPMCOUNTER28, BM_CSR_HPMCOUNTER29, BM_CSR_HPMCOUNTER30,
    BM_CSR_HPMCOUNTER31,
#endif // TARGET_HAS_HPM
};

/**
 * \brief Helper function for obtaining CSR ID corresponding to given counter
 */
static inline int bm_counter_get_csr_id(const bm_counter_id counter)
{
    if (bm_get_priv_mode() == BM_PRIV_MODE_MACHINE)
    {
        bm_fatal_check_index(counter, COUNTER_CSR_M_TABLE);

        return COUNTER_CSR_M_TABLE[counter];
    }

    bm_fatal_check_index(counter, COUNTER_CSR_TABLE);

    return COUNTER_CSR_TABLE[counter];
}

/**
 * \brief Writes a value to a CSR HPM registers using a macro resolved from the CSR name variable.
 *
 * This local inline function converts the given CSR name (as a variable) into a macro,
 * and writes the specified value to the corresponding CSR (Control and Status Register).
 *
 * \param csr Name of the CSR register (as a variable)
 * \param val Value to be written to the CSR register
 */
static inline void csr_hpm_write(bm_csr_id_t csr, xlen_t val)
{
    switch (csr)
    {
        case BM_CSR_CYCLE:
            BM_CSR_WRITE(BM_CSR_CYCLE, val);
            break;
        case BM_CSR_TIME:
            BM_CSR_WRITE(BM_CSR_TIME, val);
            break;
        case BM_CSR_INSTRET:
            BM_CSR_WRITE(BM_CSR_INSTRET, val);
            break;
        case BM_CSR_MCYCLE:
            BM_CSR_WRITE(BM_CSR_MCYCLE, val);
            break;
        case BM_CSR_MINSTRET:
            BM_CSR_WRITE(BM_CSR_MINSTRET, val);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER3, val);
            break;
        case BM_CSR_HPMCOUNTER4:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER4, val);
            break;
        case BM_CSR_HPMCOUNTER5:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER5, val);
            break;
        case BM_CSR_HPMCOUNTER6:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER6, val);
            break;
        case BM_CSR_HPMCOUNTER7:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER7, val);
            break;
        case BM_CSR_HPMCOUNTER8:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER8, val);
            break;
        case BM_CSR_HPMCOUNTER9:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER9, val);
            break;
        case BM_CSR_HPMCOUNTER10:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER10, val);
            break;
        case BM_CSR_HPMCOUNTER11:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER11, val);
            break;
        case BM_CSR_HPMCOUNTER12:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER12, val);
            break;
        case BM_CSR_HPMCOUNTER13:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER13, val);
            break;
        case BM_CSR_HPMCOUNTER14:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER14, val);
            break;
        case BM_CSR_HPMCOUNTER15:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER15, val);
            break;
        case BM_CSR_HPMCOUNTER16:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER16, val);
            break;
        case BM_CSR_HPMCOUNTER17:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER17, val);
            break;
        case BM_CSR_HPMCOUNTER18:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER18, val);
            break;
        case BM_CSR_HPMCOUNTER19:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER19, val);
            break;
        case BM_CSR_HPMCOUNTER20:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER20, val);
            break;
        case BM_CSR_HPMCOUNTER21:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER21, val);
            break;
        case BM_CSR_HPMCOUNTER22:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER22, val);
            break;
        case BM_CSR_HPMCOUNTER23:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER23, val);
            break;
        case BM_CSR_HPMCOUNTER24:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER24, val);
            break;
        case BM_CSR_HPMCOUNTER25:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER25, val);
            break;
        case BM_CSR_HPMCOUNTER26:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER26, val);
            break;
        case BM_CSR_HPMCOUNTER27:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER27, val);
            break;
        case BM_CSR_HPMCOUNTER28:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER28, val);
            break;
        case BM_CSR_HPMCOUNTER29:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER29, val);
            break;
        case BM_CSR_HPMCOUNTER30:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER30, val);
            break;
        case BM_CSR_HPMCOUNTER31:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER31, val);
            break;
        case BM_CSR_MHPMCOUNTER3:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER3, val);
            break;
        case BM_CSR_MHPMCOUNTER4:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER4, val);
            break;
        case BM_CSR_MHPMCOUNTER5:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER5, val);
            break;
        case BM_CSR_MHPMCOUNTER6:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER6, val);
            break;
        case BM_CSR_MHPMCOUNTER7:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER7, val);
            break;
        case BM_CSR_MHPMCOUNTER8:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER8, val);
            break;
        case BM_CSR_MHPMCOUNTER9:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER9, val);
            break;
        case BM_CSR_MHPMCOUNTER10:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER10, val);
            break;
        case BM_CSR_MHPMCOUNTER11:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER11, val);
            break;
        case BM_CSR_MHPMCOUNTER12:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER12, val);
            break;
        case BM_CSR_MHPMCOUNTER13:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER13, val);
            break;
        case BM_CSR_MHPMCOUNTER14:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER14, val);
            break;
        case BM_CSR_MHPMCOUNTER15:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER15, val);
            break;
        case BM_CSR_MHPMCOUNTER16:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER16, val);
            break;
        case BM_CSR_MHPMCOUNTER17:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER17, val);
            break;
        case BM_CSR_MHPMCOUNTER18:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER18, val);
            break;
        case BM_CSR_MHPMCOUNTER19:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER19, val);
            break;
        case BM_CSR_MHPMCOUNTER20:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER20, val);
            break;
        case BM_CSR_MHPMCOUNTER21:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER21, val);
            break;
        case BM_CSR_MHPMCOUNTER22:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER22, val);
            break;
        case BM_CSR_MHPMCOUNTER23:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER23, val);
            break;
        case BM_CSR_MHPMCOUNTER24:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER24, val);
            break;
        case BM_CSR_MHPMCOUNTER25:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER25, val);
            break;
        case BM_CSR_MHPMCOUNTER26:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER26, val);
            break;
        case BM_CSR_MHPMCOUNTER27:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER27, val);
            break;
        case BM_CSR_MHPMCOUNTER28:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER28, val);
            break;
        case BM_CSR_MHPMCOUNTER29:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER29, val);
            break;
        case BM_CSR_MHPMCOUNTER30:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER30, val);
            break;
        case BM_CSR_MHPMCOUNTER31:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER31, val);
            break;
#endif // TARGET_HAS_HPM
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }
}

/**
 * \brief Reads the value from CSR HPM registers using a macro resolved from the CSR name variable.
 *
 * This local inline function converts the given CSR name (as a variable) into a macro,
 * and reads the value.
 *
 * \param csr Name of the CSR register (as a variable)
 * 
 * \return Value of the CSR
 */
static inline xlen_t csr_hpm_read(bm_csr_id_t csr)
{
    xlen_t val = 0;

    switch (csr)
    {
        case BM_CSR_CYCLE:
            BM_CSR_READ(BM_CSR_CYCLE, val);
            break;
        case BM_CSR_TIME:
            BM_CSR_READ(BM_CSR_TIME, val);
            break;
        case BM_CSR_INSTRET:
            BM_CSR_READ(BM_CSR_INSTRET, val);
            break;
        case BM_CSR_MCYCLE:
            BM_CSR_READ(BM_CSR_MCYCLE, val);
            break;
        case BM_CSR_MINSTRET:
            BM_CSR_READ(BM_CSR_MINSTRET, val);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3:
            BM_CSR_READ(BM_CSR_HPMCOUNTER3, val);
            break;
        case BM_CSR_HPMCOUNTER4:
            BM_CSR_READ(BM_CSR_HPMCOUNTER4, val);
            break;
        case BM_CSR_HPMCOUNTER5:
            BM_CSR_READ(BM_CSR_HPMCOUNTER5, val);
            break;
        case BM_CSR_HPMCOUNTER6:
            BM_CSR_READ(BM_CSR_HPMCOUNTER6, val);
            break;
        case BM_CSR_HPMCOUNTER7:
            BM_CSR_READ(BM_CSR_HPMCOUNTER7, val);
            break;
        case BM_CSR_HPMCOUNTER8:
            BM_CSR_READ(BM_CSR_HPMCOUNTER8, val);
            break;
        case BM_CSR_HPMCOUNTER9:
            BM_CSR_READ(BM_CSR_HPMCOUNTER9, val);
            break;
        case BM_CSR_HPMCOUNTER10:
            BM_CSR_READ(BM_CSR_HPMCOUNTER10, val);
            break;
        case BM_CSR_HPMCOUNTER11:
            BM_CSR_READ(BM_CSR_HPMCOUNTER11, val);
            break;
        case BM_CSR_HPMCOUNTER12:
            BM_CSR_READ(BM_CSR_HPMCOUNTER12, val);
            break;
        case BM_CSR_HPMCOUNTER13:
            BM_CSR_READ(BM_CSR_HPMCOUNTER13, val);
            break;
        case BM_CSR_HPMCOUNTER14:
            BM_CSR_READ(BM_CSR_HPMCOUNTER14, val);
            break;
        case BM_CSR_HPMCOUNTER15:
            BM_CSR_READ(BM_CSR_HPMCOUNTER15, val);
            break;
        case BM_CSR_HPMCOUNTER16:
            BM_CSR_READ(BM_CSR_HPMCOUNTER16, val);
            break;
        case BM_CSR_HPMCOUNTER17:
            BM_CSR_READ(BM_CSR_HPMCOUNTER17, val);
            break;
        case BM_CSR_HPMCOUNTER18:
            BM_CSR_READ(BM_CSR_HPMCOUNTER18, val);
            break;
        case BM_CSR_HPMCOUNTER19:
            BM_CSR_READ(BM_CSR_HPMCOUNTER19, val);
            break;
        case BM_CSR_HPMCOUNTER20:
            BM_CSR_READ(BM_CSR_HPMCOUNTER20, val);
            break;
        case BM_CSR_HPMCOUNTER21:
            BM_CSR_READ(BM_CSR_HPMCOUNTER21, val);
            break;
        case BM_CSR_HPMCOUNTER22:
            BM_CSR_READ(BM_CSR_HPMCOUNTER22, val);
            break;
        case BM_CSR_HPMCOUNTER23:
            BM_CSR_READ(BM_CSR_HPMCOUNTER23, val);
            break;
        case BM_CSR_HPMCOUNTER24:
            BM_CSR_READ(BM_CSR_HPMCOUNTER24, val);
            break;
        case BM_CSR_HPMCOUNTER25:
            BM_CSR_READ(BM_CSR_HPMCOUNTER25, val);
            break;
        case BM_CSR_HPMCOUNTER26:
            BM_CSR_READ(BM_CSR_HPMCOUNTER26, val);
            break;
        case BM_CSR_HPMCOUNTER27:
            BM_CSR_READ(BM_CSR_HPMCOUNTER27, val);
            break;
        case BM_CSR_HPMCOUNTER28:
            BM_CSR_READ(BM_CSR_HPMCOUNTER28, val);
            break;
        case BM_CSR_HPMCOUNTER29:
            BM_CSR_READ(BM_CSR_HPMCOUNTER29, val);
            break;
        case BM_CSR_HPMCOUNTER30:
            BM_CSR_READ(BM_CSR_HPMCOUNTER30, val);
            break;
        case BM_CSR_HPMCOUNTER31:
            BM_CSR_READ(BM_CSR_HPMCOUNTER31, val);
            break;
        case BM_CSR_MHPMCOUNTER3:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER3, val);
            break;
        case BM_CSR_MHPMCOUNTER4:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER4, val);
            break;
        case BM_CSR_MHPMCOUNTER5:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER5, val);
            break;
        case BM_CSR_MHPMCOUNTER6:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER6, val);
            break;
        case BM_CSR_MHPMCOUNTER7:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER7, val);
            break;
        case BM_CSR_MHPMCOUNTER8:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER8, val);
            break;
        case BM_CSR_MHPMCOUNTER9:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER9, val);
            break;
        case BM_CSR_MHPMCOUNTER10:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER10, val);
            break;
        case BM_CSR_MHPMCOUNTER11:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER11, val);
            break;
        case BM_CSR_MHPMCOUNTER12:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER12, val);
            break;
        case BM_CSR_MHPMCOUNTER13:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER13, val);
            break;
        case BM_CSR_MHPMCOUNTER14:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER14, val);
            break;
        case BM_CSR_MHPMCOUNTER15:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER15, val);
            break;
        case BM_CSR_MHPMCOUNTER16:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER16, val);
            break;
        case BM_CSR_MHPMCOUNTER17:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER17, val);
            break;
        case BM_CSR_MHPMCOUNTER18:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER18, val);
            break;
        case BM_CSR_MHPMCOUNTER19:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER19, val);
            break;
        case BM_CSR_MHPMCOUNTER20:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER20, val);
            break;
        case BM_CSR_MHPMCOUNTER21:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER21, val);
            break;
        case BM_CSR_MHPMCOUNTER22:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER22, val);
            break;
        case BM_CSR_MHPMCOUNTER23:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER23, val);
            break;
        case BM_CSR_MHPMCOUNTER24:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER24, val);
            break;
        case BM_CSR_MHPMCOUNTER25:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER25, val);
            break;
        case BM_CSR_MHPMCOUNTER26:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER26, val);
            break;
        case BM_CSR_MHPMCOUNTER27:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER27, val);
            break;
        case BM_CSR_MHPMCOUNTER28:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER28, val);
            break;
        case BM_CSR_MHPMCOUNTER29:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER29, val);
            break;
        case BM_CSR_MHPMCOUNTER30:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER30, val);
            break;
        case BM_CSR_MHPMCOUNTER31:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER31, val);
            break;
#endif // TARGET_HAS_HPM
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }

    return val;
}

#if RISCV_XLEN == 32
/** \brief Helper table with CSR IDs for higher bits of machine mode counters */
static const int COUNTER_CSR_H_M_TABLE[] = {
    BM_CSR_MCYCLEH,
    BM_CSR_UNDEFINED, // not available
    BM_CSR_MINSTRETH,
    #ifdef TARGET_HAS_HPM
    BM_CSR_MHPMCOUNTER3H,  BM_CSR_MHPMCOUNTER4H,  BM_CSR_MHPMCOUNTER5H,  BM_CSR_MHPMCOUNTER6H,
    BM_CSR_MHPMCOUNTER7H,  BM_CSR_MHPMCOUNTER8H,  BM_CSR_MHPMCOUNTER9H,  BM_CSR_MHPMCOUNTER10H,
    BM_CSR_MHPMCOUNTER11H, BM_CSR_MHPMCOUNTER12H, BM_CSR_MHPMCOUNTER13H, BM_CSR_MHPMCOUNTER14H,
    BM_CSR_MHPMCOUNTER15H, BM_CSR_MHPMCOUNTER16H, BM_CSR_MHPMCOUNTER17H, BM_CSR_MHPMCOUNTER18H,
    BM_CSR_MHPMCOUNTER19H, BM_CSR_MHPMCOUNTER20H, BM_CSR_MHPMCOUNTER21H, BM_CSR_MHPMCOUNTER22H,
    BM_CSR_MHPMCOUNTER23H, BM_CSR_MHPMCOUNTER24H, BM_CSR_MHPMCOUNTER25H, BM_CSR_MHPMCOUNTER26H,
    BM_CSR_MHPMCOUNTER27H, BM_CSR_MHPMCOUNTER28H, BM_CSR_MHPMCOUNTER29H, BM_CSR_MHPMCOUNTER30H,
    BM_CSR_MHPMCOUNTER31H,
    #endif // TARGET_HAS_HPM
};

/** \brief Helper table with CSR IDs for higher bits of user/supervisor mode counters */
static const int COUNTER_CSR_H_TABLE[] = {
    BM_CSR_CYCLEH,        BM_CSR_TIMEH,         BM_CSR_INSTRETH,
    #ifdef TARGET_HAS_HPM
    BM_CSR_HPMCOUNTER3H,  BM_CSR_HPMCOUNTER4H,  BM_CSR_HPMCOUNTER5H,  BM_CSR_HPMCOUNTER6H,
    BM_CSR_HPMCOUNTER7H,  BM_CSR_HPMCOUNTER8H,  BM_CSR_HPMCOUNTER9H,  BM_CSR_HPMCOUNTER10H,
    BM_CSR_HPMCOUNTER11H, BM_CSR_HPMCOUNTER12H, BM_CSR_HPMCOUNTER13H, BM_CSR_HPMCOUNTER14H,
    BM_CSR_HPMCOUNTER15H, BM_CSR_HPMCOUNTER16H, BM_CSR_HPMCOUNTER17H, BM_CSR_HPMCOUNTER18H,
    BM_CSR_HPMCOUNTER19H, BM_CSR_HPMCOUNTER20H, BM_CSR_HPMCOUNTER21H, BM_CSR_HPMCOUNTER22H,
    BM_CSR_HPMCOUNTER23H, BM_CSR_HPMCOUNTER24H, BM_CSR_HPMCOUNTER25H, BM_CSR_HPMCOUNTER26H,
    BM_CSR_HPMCOUNTER27H, BM_CSR_HPMCOUNTER28H, BM_CSR_HPMCOUNTER29H, BM_CSR_HPMCOUNTER30H,
    BM_CSR_HPMCOUNTER31H,
    #endif // TARGET_HAS_HPM
};

/**
 * \brief Helper function for obtaining CSR ID corresponding to higher bits of given counter
 */
static inline int bm_counter_get_csr_h_id(const bm_counter_id counter)
{
    if (bm_get_priv_mode() == BM_PRIV_MODE_MACHINE)
    {
        bm_fatal_check_index(counter, COUNTER_CSR_H_M_TABLE);

        return COUNTER_CSR_H_M_TABLE[counter];
    }

    bm_fatal_check_index(counter, COUNTER_CSR_H_TABLE);

    return COUNTER_CSR_H_TABLE[counter];
}

/**
 * \brief Writes a value to a CSR HPM H registers using a macro resolved from the CSR name variable.
 *
 * This local inline function converts the given CSR name (as a variable) into a macro,
 * and writes the specified value to the corresponding CSR (Control and Status Register).
 *
 * \param csr Name of the CSR register (as a variable)
 * \param val Value to be written to the CSR register
 */
static inline void csr_hpmh_write(bm_csr_id_t csr, xlen_t val)
{
    switch (csr)
    {
        case BM_CSR_CYCLEH:
            BM_CSR_WRITE(BM_CSR_CYCLEH, val);
            break;
        case BM_CSR_TIMEH:
            BM_CSR_WRITE(BM_CSR_TIMEH, val);
            break;
        case BM_CSR_INSTRETH:
            BM_CSR_WRITE(BM_CSR_INSTRETH, val);
            break;
        case BM_CSR_MCYCLEH:
            BM_CSR_WRITE(BM_CSR_MCYCLEH, val);
            break;
        case BM_CSR_MINSTRETH:
            BM_CSR_WRITE(BM_CSR_MINSTRETH, val);
            break;
    #ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER3H, val);
            break;
        case BM_CSR_HPMCOUNTER4H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER4H, val);
            break;
        case BM_CSR_HPMCOUNTER5H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER5H, val);
            break;
        case BM_CSR_HPMCOUNTER6H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER6H, val);
            break;
        case BM_CSR_HPMCOUNTER7H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER7H, val);
            break;
        case BM_CSR_HPMCOUNTER8H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER8H, val);
            break;
        case BM_CSR_HPMCOUNTER9H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER9H, val);
            break;
        case BM_CSR_HPMCOUNTER10H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER10H, val);
            break;
        case BM_CSR_HPMCOUNTER11H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER11H, val);
            break;
        case BM_CSR_HPMCOUNTER12H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER12H, val);
            break;
        case BM_CSR_HPMCOUNTER13H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER13H, val);
            break;
        case BM_CSR_HPMCOUNTER14H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER14H, val);
            break;
        case BM_CSR_HPMCOUNTER15H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER15H, val);
            break;
        case BM_CSR_HPMCOUNTER16H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER16H, val);
            break;
        case BM_CSR_HPMCOUNTER17H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER17H, val);
            break;
        case BM_CSR_HPMCOUNTER18H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER18H, val);
            break;
        case BM_CSR_HPMCOUNTER19H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER19H, val);
            break;
        case BM_CSR_HPMCOUNTER20H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER20H, val);
            break;
        case BM_CSR_HPMCOUNTER21H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER21H, val);
            break;
        case BM_CSR_HPMCOUNTER22H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER22H, val);
            break;
        case BM_CSR_HPMCOUNTER23H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER23H, val);
            break;
        case BM_CSR_HPMCOUNTER24H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER24H, val);
            break;
        case BM_CSR_HPMCOUNTER25H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER25H, val);
            break;
        case BM_CSR_HPMCOUNTER26H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER26H, val);
            break;
        case BM_CSR_HPMCOUNTER27H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER27H, val);
            break;
        case BM_CSR_HPMCOUNTER28H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER28H, val);
            break;
        case BM_CSR_HPMCOUNTER29H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER29H, val);
            break;
        case BM_CSR_HPMCOUNTER30H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER30H, val);
            break;
        case BM_CSR_HPMCOUNTER31H:
            BM_CSR_WRITE(BM_CSR_HPMCOUNTER31H, val);
            break;
        case BM_CSR_MHPMCOUNTER3H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER3H, val);
            break;
        case BM_CSR_MHPMCOUNTER4H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER4H, val);
            break;
        case BM_CSR_MHPMCOUNTER5H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER5H, val);
            break;
        case BM_CSR_MHPMCOUNTER6H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER6H, val);
            break;
        case BM_CSR_MHPMCOUNTER7H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER7H, val);
            break;
        case BM_CSR_MHPMCOUNTER8H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER8H, val);
            break;
        case BM_CSR_MHPMCOUNTER9H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER9H, val);
            break;
        case BM_CSR_MHPMCOUNTER10H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER10H, val);
            break;
        case BM_CSR_MHPMCOUNTER11H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER11H, val);
            break;
        case BM_CSR_MHPMCOUNTER12H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER12H, val);
            break;
        case BM_CSR_MHPMCOUNTER13H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER13H, val);
            break;
        case BM_CSR_MHPMCOUNTER14H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER14H, val);
            break;
        case BM_CSR_MHPMCOUNTER15H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER15H, val);
            break;
        case BM_CSR_MHPMCOUNTER16H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER16H, val);
            break;
        case BM_CSR_MHPMCOUNTER17H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER17H, val);
            break;
        case BM_CSR_MHPMCOUNTER18H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER18H, val);
            break;
        case BM_CSR_MHPMCOUNTER19H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER19H, val);
            break;
        case BM_CSR_MHPMCOUNTER20H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER20H, val);
            break;
        case BM_CSR_MHPMCOUNTER21H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER21H, val);
            break;
        case BM_CSR_MHPMCOUNTER22H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER22H, val);
            break;
        case BM_CSR_MHPMCOUNTER23H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER23H, val);
            break;
        case BM_CSR_MHPMCOUNTER24H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER24H, val);
            break;
        case BM_CSR_MHPMCOUNTER25H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER25H, val);
            break;
        case BM_CSR_MHPMCOUNTER26H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER26H, val);
            break;
        case BM_CSR_MHPMCOUNTER27H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER27H, val);
            break;
        case BM_CSR_MHPMCOUNTER28H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER28H, val);
            break;
        case BM_CSR_MHPMCOUNTER29H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER29H, val);
            break;
        case BM_CSR_MHPMCOUNTER30H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER30H, val);
            break;
        case BM_CSR_MHPMCOUNTER31H:
            BM_CSR_WRITE(BM_CSR_MHPMCOUNTER31H, val);
            break;
    #endif // TARGET_HAS_HPM
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }
}

/**
 * \brief Reads the value from CSR HPM H registers using a macro resolved from the CSR name variable.
 *
 * This local inline function converts the given CSR name (as a variable) into a macro,
 * and reads the value.
 *
 * \param csr Name of the CSR register (as a variable)
 * 
 * \return Value of the CSR
 */
static inline xlen_t csr_hpmh_read(bm_csr_id_t csr)
{
    xlen_t val = 0;

    switch (csr)
    {
        case BM_CSR_CYCLEH:
            BM_CSR_READ(BM_CSR_CYCLEH, val);
            break;
        case BM_CSR_TIMEH:
            BM_CSR_READ(BM_CSR_TIMEH, val);
            break;
        case BM_CSR_INSTRETH:
            BM_CSR_READ(BM_CSR_INSTRETH, val);
            break;
        case BM_CSR_MCYCLEH:
            BM_CSR_READ(BM_CSR_MCYCLEH, val);
            break;
        case BM_CSR_MINSTRETH:
            BM_CSR_READ(BM_CSR_MINSTRETH, val);
            break;
    #ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER3H, val);
            break;
        case BM_CSR_HPMCOUNTER4H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER4H, val);
            break;
        case BM_CSR_HPMCOUNTER5H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER5H, val);
            break;
        case BM_CSR_HPMCOUNTER6H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER6H, val);
            break;
        case BM_CSR_HPMCOUNTER7H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER7H, val);
            break;
        case BM_CSR_HPMCOUNTER8H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER8H, val);
            break;
        case BM_CSR_HPMCOUNTER9H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER9H, val);
            break;
        case BM_CSR_HPMCOUNTER10H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER10H, val);
            break;
        case BM_CSR_HPMCOUNTER11H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER11H, val);
            break;
        case BM_CSR_HPMCOUNTER12H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER12H, val);
            break;
        case BM_CSR_HPMCOUNTER13H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER13H, val);
            break;
        case BM_CSR_HPMCOUNTER14H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER14H, val);
            break;
        case BM_CSR_HPMCOUNTER15H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER15H, val);
            break;
        case BM_CSR_HPMCOUNTER16H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER16H, val);
            break;
        case BM_CSR_HPMCOUNTER17H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER17H, val);
            break;
        case BM_CSR_HPMCOUNTER18H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER18H, val);
            break;
        case BM_CSR_HPMCOUNTER19H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER19H, val);
            break;
        case BM_CSR_HPMCOUNTER20H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER20H, val);
            break;
        case BM_CSR_HPMCOUNTER21H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER21H, val);
            break;
        case BM_CSR_HPMCOUNTER22H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER22H, val);
            break;
        case BM_CSR_HPMCOUNTER23H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER23H, val);
            break;
        case BM_CSR_HPMCOUNTER24H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER24H, val);
            break;
        case BM_CSR_HPMCOUNTER25H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER25H, val);
            break;
        case BM_CSR_HPMCOUNTER26H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER26H, val);
            break;
        case BM_CSR_HPMCOUNTER27H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER27H, val);
            break;
        case BM_CSR_HPMCOUNTER28H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER28H, val);
            break;
        case BM_CSR_HPMCOUNTER29H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER29H, val);
            break;
        case BM_CSR_HPMCOUNTER30H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER30H, val);
            break;
        case BM_CSR_HPMCOUNTER31H:
            BM_CSR_READ(BM_CSR_HPMCOUNTER31H, val);
            break;
        case BM_CSR_MHPMCOUNTER3H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER3H, val);
            break;
        case BM_CSR_MHPMCOUNTER4H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER4H, val);
            break;
        case BM_CSR_MHPMCOUNTER5H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER5H, val);
            break;
        case BM_CSR_MHPMCOUNTER6H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER6H, val);
            break;
        case BM_CSR_MHPMCOUNTER7H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER7H, val);
            break;
        case BM_CSR_MHPMCOUNTER8H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER8H, val);
            break;
        case BM_CSR_MHPMCOUNTER9H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER9H, val);
            break;
        case BM_CSR_MHPMCOUNTER10H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER10H, val);
            break;
        case BM_CSR_MHPMCOUNTER11H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER11H, val);
            break;
        case BM_CSR_MHPMCOUNTER12H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER12H, val);
            break;
        case BM_CSR_MHPMCOUNTER13H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER13H, val);
            break;
        case BM_CSR_MHPMCOUNTER14H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER14H, val);
            break;
        case BM_CSR_MHPMCOUNTER15H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER15H, val);
            break;
        case BM_CSR_MHPMCOUNTER16H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER16H, val);
            break;
        case BM_CSR_MHPMCOUNTER17H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER17H, val);
            break;
        case BM_CSR_MHPMCOUNTER18H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER18H, val);
            break;
        case BM_CSR_MHPMCOUNTER19H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER19H, val);
            break;
        case BM_CSR_MHPMCOUNTER20H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER20H, val);
            break;
        case BM_CSR_MHPMCOUNTER21H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER21H, val);
            break;
        case BM_CSR_MHPMCOUNTER22H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER22H, val);
            break;
        case BM_CSR_MHPMCOUNTER23H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER23H, val);
            break;
        case BM_CSR_MHPMCOUNTER24H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER24H, val);
            break;
        case BM_CSR_MHPMCOUNTER25H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER25H, val);
            break;
        case BM_CSR_MHPMCOUNTER26H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER26H, val);
            break;
        case BM_CSR_MHPMCOUNTER27H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER27H, val);
            break;
        case BM_CSR_MHPMCOUNTER28H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER28H, val);
            break;
        case BM_CSR_MHPMCOUNTER29H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER29H, val);
            break;
        case BM_CSR_MHPMCOUNTER30H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER30H, val);
            break;
        case BM_CSR_MHPMCOUNTER31H:
            BM_CSR_READ(BM_CSR_MHPMCOUNTER31H, val);
            break;
    #endif // TARGET_HAS_HPM
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }

    return val;
}
#endif

uint64_t bm_counter_read(const bm_counter_id counter)
{
    int csr = bm_counter_get_csr_id(counter);
#if RISCV_XLEN == 32
    // Counters are split in two CSRs for 32-bit targets, the following sequence ensures
    // correct reading even if the counter increments in between the CSR accesses.
    int csrh = bm_counter_get_csr_h_id(counter);

    uint32_t low, high, high_tmp;
    do
    {
        high_tmp = csr_hpmh_read(csrh);
        low      = csr_hpm_read(csr);
        high     = csr_hpmh_read(csrh);
    } while (high_tmp != high);
    return ((uint64_t)high << 32) | low;
#else
    return csr_hpm_read(csr);
#endif
}

void bm_counter_clear(const bm_counter_id counter)
{
    int csr = bm_counter_get_csr_id(counter);
    csr_hpm_write(csr, 0);
#if RISCV_XLEN == 32
    int csrh = bm_counter_get_csr_h_id(counter);
    csr_hpmh_write(csrh, 0);
#endif
}

#if defined(TARGET_EXT_U) || defined(TARGET_EXT_S)
void bm_counter_enable_priv(bm_priv_mode_t priv_mode, const bm_counter_id counter)
{
    if (bm_get_priv_mode() <= priv_mode)
    {
        bm_fatal("Cannot enable for higher mode.");
    }

    xlen_t mask = 1 << counter;

    if (bm_get_priv_mode() == BM_PRIV_MODE_MACHINE)
    {
        BM_CSR_SET(BM_CSR_MCOUNTEREN, mask);
    }

    #if defined(TARGET_EXT_U) && defined(TARGET_EXT_S)
    if (priv_mode == BM_PRIV_MODE_USER)
    {
        BM_CSR_SET(BM_CSR_SCOUNTEREN, mask);
    }
    #endif
}
#endif

void bm_counter_pause(const bm_counter_id counter)
{
    if (bm_get_priv_mode() != BM_PRIV_MODE_MACHINE)
    {
        bm_fatal("Must run in machine mode.");
    }

    xlen_t mask = 1 << counter;
    BM_CSR_SET(BM_CSR_MCOUNTINHIBIT, mask);
}

void bm_counter_resume(const bm_counter_id counter)
{
    if (bm_get_priv_mode() != BM_PRIV_MODE_MACHINE)
    {
        bm_fatal("Must run in machine mode.");
    }

    xlen_t mask = 1 << counter;
    BM_CSR_CLEAR(BM_CSR_MCOUNTINHIBIT, mask);
}

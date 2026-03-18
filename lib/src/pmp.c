/* Copyright 2023-2025 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/pmp.h"

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/mem_barrier.h"
#include "baremetal/verbose.h"

/** \brief Helper table with PMP address CSR IDs */
static const bm_csr_id_t pmp_address_table[TARGET_PMP_NUM_REGIONS] = {
    BM_CSR_PMPADDR0,
    BM_CSR_PMPADDR1,
    BM_CSR_PMPADDR2,
    BM_CSR_PMPADDR3,
#if TARGET_PMP_NUM_REGIONS > 4
    BM_CSR_PMPADDR4,
    BM_CSR_PMPADDR5,
    BM_CSR_PMPADDR6,
    BM_CSR_PMPADDR7,
#endif // TARGET_PMP_NUM_REGIONS > 4
#if TARGET_PMP_NUM_REGIONS > 8
    BM_CSR_PMPADDR8,
    BM_CSR_PMPADDR9,
    BM_CSR_PMPADDR10,
    BM_CSR_PMPADDR11,
#endif // TARGET_PMP_NUM_REGIONS > 8
#if TARGET_PMP_NUM_REGIONS > 12
    BM_CSR_PMPADDR12,
    BM_CSR_PMPADDR13,
    BM_CSR_PMPADDR14,
    BM_CSR_PMPADDR15,
#endif // TARGET_PMP_NUM_REGIONS > 12
};

/** \brief Helper table with PMP config CSR IDs */
static const bm_csr_id_t pmp_config_table[] = {
    BM_CSR_PMPCFG0,
#if TARGET_HAS_PMP_CFG_REG1
    BM_CSR_PMPCFG1,
#endif // TARGET_HAS_PMP_CFG_REG1
#if TARGET_HAS_PMP_CFG_REG2
    BM_CSR_PMPCFG2,
#endif // TARGET_HAS_PMP_CFG_REG2
#if TARGET_HAS_PMP_CFG_REG3
    BM_CSR_PMPCFG3,
#endif // TARGET_HAS_PMP_CFG_REG3
};

/**
 * \brief Writes a value to a CSR PMP ADDRESS register using a macro resolved from the CSR name variable.
 *
 * This local inline function converts the given CSR name (as a variable) into a macro,
 * and writes the specified value to the corresponding CSR (Control and Status Register).
 *
 * \param csr Name of the CSR register (as a variable)
 * \param val Value to be written to the CSR register
 */
static inline void csr_pmp_address_write(bm_csr_id_t csr, xlen_t val)
{
    switch (csr)
    {
        case BM_CSR_PMPADDR0:
            BM_CSR_WRITE(BM_CSR_PMPADDR0, val);
            break;
        case BM_CSR_PMPADDR1:
            BM_CSR_WRITE(BM_CSR_PMPADDR1, val);
            break;
        case BM_CSR_PMPADDR2:
            BM_CSR_WRITE(BM_CSR_PMPADDR2, val);
            break;
        case BM_CSR_PMPADDR3:
            BM_CSR_WRITE(BM_CSR_PMPADDR3, val);
            break;
#if TARGET_PMP_NUM_REGIONS > 4
        case BM_CSR_PMPADDR4:
            BM_CSR_WRITE(BM_CSR_PMPADDR4, val);
            break;
        case BM_CSR_PMPADDR5:
            BM_CSR_WRITE(BM_CSR_PMPADDR5, val);
            break;
        case BM_CSR_PMPADDR6:
            BM_CSR_WRITE(BM_CSR_PMPADDR6, val);
            break;
        case BM_CSR_PMPADDR7:
            BM_CSR_WRITE(BM_CSR_PMPADDR7, val);
            break;
#endif // TARGET_PMP_NUM_REGIONS > 4
#if TARGET_PMP_NUM_REGIONS > 8
        case BM_CSR_PMPADDR8:
            BM_CSR_WRITE(BM_CSR_PMPADDR8, val);
            break;
        case BM_CSR_PMPADDR9:
            BM_CSR_WRITE(BM_CSR_PMPADDR9, val);
            break;
        case BM_CSR_PMPADDR10:
            BM_CSR_WRITE(BM_CSR_PMPADDR10, val);
            break;
        case BM_CSR_PMPADDR11:
            BM_CSR_WRITE(BM_CSR_PMPADDR11, val);
            break;
#endif // TARGET_PMP_NUM_REGIONS > 8
#if TARGET_PMP_NUM_REGIONS > 12
        case BM_CSR_PMPADDR12:
            BM_CSR_WRITE(BM_CSR_PMPADDR12, val);
            break;
        case BM_CSR_PMPADDR13:
            BM_CSR_WRITE(BM_CSR_PMPADDR13, val);
            break;
        case BM_CSR_PMPADDR14:
            BM_CSR_WRITE(BM_CSR_PMPADDR14, val);
            break;
        case BM_CSR_PMPADDR15:
            BM_CSR_WRITE(BM_CSR_PMPADDR15, val);
            break;
#endif // TARGET_PMP_NUM_REGIONS > 12
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
    }
}

/**
 * \brief Clears bits in a specified PMP CFG CSR register using a bitmask.
 *
 * This local inline function takes a CSR register identifier and applies a bitwise set operation
 * using the provided mask value. It uses predefined macros to access and modify the appropriate CSR.
 *
 * \param csr The CSR register identifier (of type bm_csr_id_t)
 * \param mask Bitmask indicating which bits to clear in the specified CSR
 */
static inline void csr_pmp_config_clear(bm_csr_id_t csr, xlen_t mask)
{
    switch (csr)
    {
        case BM_CSR_PMPCFG0:
            BM_CSR_CLEAR(BM_CSR_PMPCFG0, mask);
            break;
#if TARGET_HAS_PMP_CFG_REG1
        case BM_CSR_PMPCFG1:
            BM_CSR_CLEAR(BM_CSR_PMPCFG1, mask);
            break;
#endif // TARGET_HAS_PMP_CFG_REG1
#if TARGET_HAS_PMP_CFG_REG2
        case BM_CSR_PMPCFG2:
            BM_CSR_CLEAR(BM_CSR_PMPCFG2, mask);
            break;
#endif // TARGET_HAS_PMP_CFG_REG2
#if TARGET_HAS_PMP_CFG_REG3
        case BM_CSR_PMPCFG3:
            BM_CSR_CLEAR(BM_CSR_PMPCFG3, mask);
            break;
#endif // TARGET_HAS_PMP_CFG_REG3
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
    }
}

/**
 * \brief Sets bits in a specified PMP CFG CSR register using a bitmask.
 *
 * This local inline function takes a CSR register identifier and applies a bitwise set operation
 * using the provided mask value. It uses predefined macros to access and modify the appropriate CSR.
 *
 * \param csr The CSR register identifier (of type bm_csr_id_t)
 * \param mask Bitmask indicating which bits to set in the specified CSR
 */
static inline void csr_pmp_config_set(bm_csr_id_t csr, xlen_t mask)
{
    switch (csr)
    {
        case BM_CSR_PMPCFG0:
            BM_CSR_SET(BM_CSR_PMPCFG0, mask);
            break;
#if TARGET_HAS_PMP_CFG_REG1
        case BM_CSR_PMPCFG1:
            BM_CSR_SET(BM_CSR_PMPCFG1, mask);
            break;
#endif // #if TARGET_HAS_PMP_CFG_REG1
#if TARGET_HAS_PMP_CFG_REG2
        case BM_CSR_PMPCFG2:
            BM_CSR_SET(BM_CSR_PMPCFG2, mask);
            break;
#endif // #if TARGET_HAS_PMP_CFG_REG2
#if TARGET_HAS_PMP_CFG_REG3
        case BM_CSR_PMPCFG3:
            BM_CSR_SET(BM_CSR_PMPCFG3, mask);
            break;
#endif // #if TARGET_HAS_PMP_CFG_REG3
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
    }
}

#define BM_PMP_CFG_SIZE 8
#define BM_PMP_CFG_MASK 0xff

void bm_pmp_set(unsigned index, xlen_t address, bm_pmp_address_mode_t address_mode, unsigned config)
{
    bm_fatal_check_index(index, pmp_address_table);

    unsigned config_index      = index / (RISCV_XLEN / BM_PMP_CFG_SIZE);
    unsigned config_reg_offset = BM_PMP_CFG_SIZE * (index % (RISCV_XLEN / BM_PMP_CFG_SIZE));

    bm_fatal_check_index(config_index, pmp_config_table);

    xlen_t config_val = config | address_mode;

    if ((config_val & BM_PMP_MODE_WRITE) && !(config_val & BM_PMP_CONFIG_READ))
    {
        bm_warn("Reserved combination R=0,W=1 (R=1 will be written).");
    }

    if ((address_mode == BM_PMP_MODE_NA4) || (address_mode == BM_PMP_MODE_NAPOT))
    {
        bm_fatal("Unsupported address mode.");
    }

    // Set address, stored shifted by 2
    csr_pmp_address_write(pmp_address_table[index], address >> 2);

    // Clear and then set the PMP configuration
    csr_pmp_config_clear(pmp_config_table[config_index], (xlen_t)BM_PMP_CFG_MASK << config_reg_offset);
    csr_pmp_config_set(pmp_config_table[config_index], config_val << config_reg_offset);

#ifdef TARGET_EXT_S
    // Flush PMP translation caching structures
    bm_exec_sfence_vma();
#endif
}

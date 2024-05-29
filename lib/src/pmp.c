/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/pmp.h"

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/mem_barrier.h"
#include "baremetal/verbose.h"

/** \brief Helper table with PMP address CSR IDs */
const bm_csr_id pmp_address_table[] = {
    BM_CSR_PMPADDR0,
    BM_CSR_PMPADDR1,
    BM_CSR_PMPADDR2,
    BM_CSR_PMPADDR3,
    BM_CSR_PMPADDR4,
    BM_CSR_PMPADDR5,
    BM_CSR_PMPADDR6,
    BM_CSR_PMPADDR7,
    BM_CSR_PMPADDR8,
    BM_CSR_PMPADDR9,
    BM_CSR_PMPADDR10,
    BM_CSR_PMPADDR11,
    BM_CSR_PMPADDR12,
    BM_CSR_PMPADDR13,
    BM_CSR_PMPADDR14,
    BM_CSR_PMPADDR15,
};

/** \brief Helper table with PMP config CSR IDs */
const bm_csr_id pmp_config_table[] = {
    BM_CSR_PMPCFG0,
    BM_CSR_PMPCFG1,
    BM_CSR_PMPCFG2,
    BM_CSR_PMPCFG3,
};

#define BM_PMP_CFG_SIZE 8
#define BM_PMP_CFG_MASK 0xff

void bm_pmp_set(unsigned index, xlen_t address, bm_pmp_address_mode_t address_mode, unsigned config)
{
    if (index >= TARGET_PMP_NUM_REGIONS)
    {
        bm_error("PMP entry index out of range.");
    }

    unsigned config_index      = index / (__riscv_xlen / BM_PMP_CFG_SIZE);
    unsigned config_reg_offset = BM_PMP_CFG_SIZE * (index % (__riscv_xlen / BM_PMP_CFG_SIZE));

    xlen_t config_val = config | address_mode;

    if ((config_val & BM_PMP_MODE_WRITE) && !(config_val & BM_PMP_CONFIG_READ))
    {
        bm_warn("Reserved combination R=0,W=1 (R=1 will be written).");
    }

    if ((address_mode == BM_PMP_MODE_NA4) || (address_mode == BM_PMP_MODE_NAPOT))
    {
        bm_error("Unsupported address mode.");
    }

    // Set address, stored shifted by 2
    bm_csr_write(pmp_address_table[index], address >> 2);

    // Clear and then set the PMP configuration
    bm_csr_clear_mask(pmp_config_table[config_index], (xlen_t)BM_PMP_CFG_MASK << config_reg_offset);
    bm_csr_set_mask(pmp_config_table[config_index], config_val << config_reg_offset);

#ifdef TARGET_EXT_S
    // Flush PMP translation caching structures
    bm_exec_sfence_vma();
#endif
}

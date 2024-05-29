/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/pic.h"

#include "baremetal/csr.h"

#include <stdbool.h>

#define PIC_REG_WIDTH 32
#define PIC_REG_MASK  0xffffffff

#define PIC_NUM_REGS  (TARGET_PIC_NUM_INTERRUPTS / PIC_REG_WIDTH)

/** \brief Helper table with PIC mask CSR IDs */
const bm_csr_id mpicmask_table[] = {
    BM_CSR_MPICMASK,
    BM_CSR_MPICMASK1,
    BM_CSR_MPICMASK2,
    BM_CSR_MPICMASK3,
};

/** \brief Helper table with PIC flag CSR IDs */
const bm_csr_id mpicflag_table[] = {
    BM_CSR_MPICFLAG,
    BM_CSR_MPICFLAG1,
    BM_CSR_MPICFLAG2,
    BM_CSR_MPICFLAG3,
};

void bm_pic_enable_source(unsigned ext_irq_id)
{
    unsigned reg_index  = ext_irq_id / PIC_REG_WIDTH;
    unsigned reg_offset = ext_irq_id % PIC_REG_WIDTH;

    bm_csr_set_mask(mpicmask_table[reg_index], 1UL << reg_offset);
}

void bm_pic_disable_source(unsigned ext_irq_id)
{
    unsigned reg_index  = ext_irq_id / PIC_REG_WIDTH;
    unsigned reg_offset = ext_irq_id % PIC_REG_WIDTH;

    bm_csr_clear_mask(mpicmask_table[reg_index], 1UL << reg_offset);
}

void bm_pic_enable_all(void)
{
    for (int i = 0; i < PIC_NUM_REGS; ++i)
    {
        bm_csr_write(mpicmask_table[i], PIC_REG_MASK);
    }
}

void bm_pic_disable_all(void)
{
    for (int i = 0; i < PIC_NUM_REGS; ++i)
    {
        bm_csr_write(mpicmask_table[i], 0);
    }
}

void bm_pic_clear_irq(unsigned bit)
{
    unsigned reg_index  = bit / PIC_REG_WIDTH;
    unsigned reg_offset = bit % PIC_REG_WIDTH;

    bm_csr_clear_mask(mpicflag_table[reg_index], 1UL << reg_offset);
}

int bm_pic_get_irq(void)
{
    for (int i = 0; i < PIC_NUM_REGS; ++i)
    {
        xlen_t enabled = bm_csr_read(mpicflag_table[i]) & bm_csr_read(mpicmask_table[i]);
        if (!enabled)
        {
            continue;
        }

        unsigned lsb = i * PIC_REG_WIDTH;
        while (!(enabled & 1))
        {
            enabled >>= 1;
            ++lsb;
        }
        return lsb;
    }

    return -1;
}

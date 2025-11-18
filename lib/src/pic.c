/* Copyright 2023-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/pic.h"

#include "baremetal/csr.h"
#include "baremetal/verbose.h"

#include <stdbool.h>

#define PIC_REG_WIDTH 32
#define PIC_REG_MASK  0xffffffff

#define PIC_NUM_REGS  (TARGET_PIC_NUM_INTERRUPTS / PIC_REG_WIDTH)

/** \brief Helper table with PIC mask CSR IDs */
static const bm_csr_id_t mpicmask_table[PIC_NUM_REGS] = {
    BM_CSR_MPICMASK,
#if PIC_NUM_REGS > 1
    BM_CSR_MPICMASK1,
#endif // PIC_NUM_REGS > 1
#if PIC_NUM_REGS > 2
    BM_CSR_MPICMASK2,
#endif // PIC_NUM_REGS > 2
#if PIC_NUM_REGS > 3
    BM_CSR_MPICMASK3,
#endif // PIC_NUM_REGS > 3
};

/** \brief Helper table with PIC flag CSR IDs */
static const bm_csr_id_t mpicflag_table[PIC_NUM_REGS] = {
    BM_CSR_MPICFLAG,
#if PIC_NUM_REGS > 1
    BM_CSR_MPICFLAG1,
#endif // PIC_NUM_REGS > 1
#if PIC_NUM_REGS > 2
    BM_CSR_MPICFLAG2,
#endif // #endif // PIC_NUM_REGS > 2
#if PIC_NUM_REGS > 3
    BM_CSR_MPICFLAG3,
#endif // #endif // PIC_NUM_REGS > 3
};

/**
 * \brief Clears bits in a specified MPIC FLAG/MASK CSR register using a bitmask.
 *
 * This local inline function takes a CSR register identifier and applies a bitwise set operation
 * using the provided mask value. It uses predefined macros to access and modify the appropriate CSR.
 *
 * \param csr The CSR register identifier (of type bm_csr_id_t)
 * \param mask Bitmask indicating which bits to clear in the specified CSR
 */
static inline void csr_mpic_clear(bm_csr_id_t csr, xlen_t mask)
{
    switch (csr)
    {
        case BM_CSR_MPICFLAG:
            BM_CSR_CLEAR(BM_CSR_MPICFLAG, mask);
            break;
        case BM_CSR_MPICMASK:
            BM_CSR_CLEAR(BM_CSR_MPICMASK, mask);
            break;
#if PIC_NUM_REGS > 1
        case BM_CSR_MPICFLAG1:
            BM_CSR_CLEAR(BM_CSR_MPICFLAG1, mask);
            break;
        case BM_CSR_MPICMASK1:
            BM_CSR_CLEAR(BM_CSR_MPICMASK1, mask);
            break;
#endif // PIC_NUM_REGS > 1
#if PIC_NUM_REGS > 2
        case BM_CSR_MPICFLAG2:
            BM_CSR_CLEAR(BM_CSR_MPICFLAG2, mask);
            break;
        case BM_CSR_MPICMASK2:
            BM_CSR_CLEAR(BM_CSR_MPICMASK2, mask);
            break;
#endif // PIC_NUM_REGS > 2
#if PIC_NUM_REGS > 3
        case BM_CSR_MPICFLAG3:
            BM_CSR_CLEAR(BM_CSR_MPICFLAG3, mask);
            break;
        case BM_CSR_MPICMASK3:
            BM_CSR_CLEAR(BM_CSR_MPICMASK3, mask);
            break;
#endif // PIC_NUM_REGS > 3
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }
}

/**
 * \brief Sets bits in a specified MPIC FLAG/MASK CSR register using a bitmask.
 *
 * This local inline function takes a CSR register identifier and applies a bitwise set operation
 * using the provided mask value. It uses predefined macros to access and modify the appropriate CSR.
 *
 * \param csr The CSR register identifier (of type bm_csr_id_t)
 * \param mask Bitmask indicating which bits to set in the specified CSR
 */
static inline void csr_mpic_set(bm_csr_id_t csr, xlen_t mask)
{
    switch (csr)
    {
        case BM_CSR_MPICFLAG:
            BM_CSR_SET(BM_CSR_MPICFLAG, mask);
            break;
        case BM_CSR_MPICMASK:
            BM_CSR_SET(BM_CSR_MPICMASK, mask);
            break;
#if PIC_NUM_REGS > 1
        case BM_CSR_MPICFLAG1:
            BM_CSR_SET(BM_CSR_MPICFLAG1, mask);
            break;
        case BM_CSR_MPICMASK1:
            BM_CSR_SET(BM_CSR_MPICMASK1, mask);
            break;
#endif // PIC_NUM_REGS > 1
#if PIC_NUM_REGS > 2
        case BM_CSR_MPICFLAG2:
            BM_CSR_SET(BM_CSR_MPICFLAG2, mask);
            break;
        case BM_CSR_MPICMASK2:
            BM_CSR_SET(BM_CSR_MPICMASK2, mask);
            break;
#endif // PIC_NUM_REGS > 2
#if PIC_NUM_REGS > 3
        case BM_CSR_MPICFLAG3:
            BM_CSR_SET(BM_CSR_MPICFLAG3, mask);
            break;
        case BM_CSR_MPICMASK3:
            BM_CSR_SET(BM_CSR_MPICMASK3, mask);
            break;
#endif // PIC_NUM_REGS > 3
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }
}

/**
 * \brief Writes a value to a MPIC FLAG/MASK register using a macro resolved from the CSR name variable.
 *
 * This local inline function converts the given CSR name (as a variable) into a macro,
 * and writes the specified value to the corresponding CSR (Control and Status Register).
 *
 * \param csr Name of the CSR register (as a variable)
 * \param val Value to be written to the CSR register
 */
static inline void csr_mpic_write(bm_csr_id_t csr, xlen_t val)
{
    switch (csr)
    {
        case BM_CSR_MPICFLAG:
            BM_CSR_WRITE(BM_CSR_MPICFLAG, val);
            break;
        case BM_CSR_MPICMASK:
            BM_CSR_WRITE(BM_CSR_MPICMASK, val);
            break;
#if PIC_NUM_REGS > 1
        case BM_CSR_MPICFLAG1:
            BM_CSR_WRITE(BM_CSR_MPICFLAG1, val);
            break;
        case BM_CSR_MPICMASK1:
            BM_CSR_WRITE(BM_CSR_MPICMASK1, val);
            break;
#endif // PIC_NUM_REGS > 1
#if PIC_NUM_REGS > 2
        case BM_CSR_MPICFLAG2:
            BM_CSR_WRITE(BM_CSR_MPICFLAG2, val);
            break;
        case BM_CSR_MPICMASK2:
            BM_CSR_WRITE(BM_CSR_MPICMASK2, val);
            break;
#endif // PIC_NUM_REGS > 2
#if PIC_NUM_REGS > 3
        case BM_CSR_MPICFLAG3:
            BM_CSR_WRITE(BM_CSR_MPICFLAG3, val);
            break;
        case BM_CSR_MPICMASK3:
            BM_CSR_WRITE(BM_CSR_MPICMASK3, val);
            break;
#endif // PIC_NUM_REGS > 3
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }
}

/**
 * \brief Reads the value from CSR MPIC FLAG/MASK registers using a macro resolved from the CSR name variable.
 *
 * This local inline function converts the given CSR name (as a variable) into a macro,
 * and reads the value.
 *
 * \param csr Name of the CSR register (as a variable)
 * 
 * \return Value of the CSR
 */
static inline xlen_t csr_mpic_read(bm_csr_id_t csr)
{
    xlen_t val = 0;

    switch (csr)
    {
        case BM_CSR_MPICFLAG:
            BM_CSR_READ(BM_CSR_MPICFLAG, val);
            break;
        case BM_CSR_MPICMASK:
            BM_CSR_READ(BM_CSR_MPICMASK, val);
            break;
#if PIC_NUM_REGS > 1
        case BM_CSR_MPICFLAG1:
            BM_CSR_READ(BM_CSR_MPICFLAG1, val);
            break;
        case BM_CSR_MPICMASK1:
            BM_CSR_READ(BM_CSR_MPICMASK1, val);
            break;
#endif // PIC_NUM_REGS > 1
#if PIC_NUM_REGS > 2
        case BM_CSR_MPICFLAG2:
            BM_CSR_READ(BM_CSR_MPICFLAG2, val);
            break;
        case BM_CSR_MPICMASK2:
            BM_CSR_READ(BM_CSR_MPICMASK2, val);
            break;
#endif // PIC_NUM_REGS > 2
#if PIC_NUM_REGS > 3
        case BM_CSR_MPICFLAG3:
            BM_CSR_READ(BM_CSR_MPICFLAG3, val);
            break;
        case BM_CSR_MPICMASK3:
            BM_CSR_READ(BM_CSR_MPICMASK3, val);
            break;
#endif // PIC_NUM_REGS > 3
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }

    return val;
}

void bm_pic_enable_source(unsigned ext_irq_id)
{
    unsigned reg_index  = ext_irq_id / PIC_REG_WIDTH;
    unsigned reg_offset = ext_irq_id % PIC_REG_WIDTH;

    bm_fatal_check_index(reg_index, mpicmask_table);

    csr_mpic_set(mpicmask_table[reg_index], 1UL << reg_offset);
}

void bm_pic_disable_source(unsigned ext_irq_id)
{
    unsigned reg_index  = ext_irq_id / PIC_REG_WIDTH;
    unsigned reg_offset = ext_irq_id % PIC_REG_WIDTH;

    bm_fatal_check_index(reg_index, mpicmask_table);

    csr_mpic_clear(mpicmask_table[reg_index], 1UL << reg_offset);
}

void bm_pic_enable_all(void)
{
    for (int i = 0; i < PIC_NUM_REGS; ++i)
    {
        csr_mpic_write(mpicmask_table[i], PIC_REG_MASK);
    }
}

void bm_pic_disable_all(void)
{
    for (int i = 0; i < PIC_NUM_REGS; ++i)
    {
        csr_mpic_write(mpicmask_table[i], 0);
    }
}

void bm_pic_clear_irq(unsigned bit)
{
    unsigned reg_index  = bit / PIC_REG_WIDTH;
    unsigned reg_offset = bit % PIC_REG_WIDTH;

    bm_fatal_check_index(reg_index, mpicflag_table);

    csr_mpic_clear(mpicflag_table[reg_index], 1UL << reg_offset);
}

int bm_pic_get_irq(void)
{
    for (int i = 0; i < PIC_NUM_REGS; ++i)
    {
        xlen_t enabled = csr_mpic_read(mpicflag_table[i]) & csr_mpic_read(mpicmask_table[i]);
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

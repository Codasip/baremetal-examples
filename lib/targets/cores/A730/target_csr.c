/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/verbose.h"

xlen_t target_csr_read(const int csr)
{
    xlen_t value = 0;

    switch (csr)
    {
        case BM_CSR_SBPREDCTRL:
            CSR_READ(BM_CSR_SBPREDCTRL, value);
            break;
        case BM_CSR_MCFGCTRL:
            CSR_READ(BM_CSR_MCFGCTRL, value);
            break;
        case BM_CSR_MDCACHECTRL:
            CSR_READ(BM_CSR_MDCACHECTRL, value);
            break;
        case BM_CSR_MICACHECTRL:
            CSR_READ(BM_CSR_MICACHECTRL, value);
            break;
        case BM_CSR_ML2CACHECTRL:
            CSR_READ(BM_CSR_ML2CACHECTRL, value);
            break;
        case BM_CSR_MTLBCTRL:
            CSR_READ(BM_CSR_MTLBCTRL, value);
            break;
        case BM_CSR_MCACHESTATUS:
            CSR_READ(BM_CSR_MCACHESTATUS, value);
            break;
#ifdef CONFIG_HAS_TCM
        case BM_CSR_MTCMCFG:
            CSR_READ(BM_CSR_MTCMCFG, value);
            break;
        case BM_CSR_MITCMBASEADDR:
            CSR_READ(BM_CSR_MITCMBASEADDR, value);
            break;
        case BM_CSR_MDTCMBASEADDR:
            CSR_READ(BM_CSR_MDTCMBASEADDR, value);
            break;
#endif
        default:
            bm_error("Unsupported CSR.");
    }
    return value;
}

void target_csr_write(const int csr, const xlen_t value)
{
    switch (csr)
    {
        case BM_CSR_SBPREDCTRL:
            CSR_WRITE(BM_CSR_SBPREDCTRL, value);
            break;
        case BM_CSR_MCFGCTRL:
            CSR_WRITE(BM_CSR_MCFGCTRL, value);
            break;
        case BM_CSR_MDCACHECTRL:
            CSR_WRITE(BM_CSR_MDCACHECTRL, value);
            break;
        case BM_CSR_MICACHECTRL:
            CSR_WRITE(BM_CSR_MICACHECTRL, value);
            break;
        case BM_CSR_ML2CACHECTRL:
            CSR_WRITE(BM_CSR_ML2CACHECTRL, value);
            break;
        case BM_CSR_MTLBCTRL:
            CSR_WRITE(BM_CSR_MTLBCTRL, value);
            break;
        case BM_CSR_MCACHESTATUS:
            CSR_WRITE(BM_CSR_MCACHESTATUS, value);
            break;
#ifdef CONFIG_HAS_TCM
        case BM_CSR_MTCMCFG:
            CSR_WRITE(BM_CSR_MTCMCFG, value);
            break;
        case BM_CSR_MITCMBASEADDR:
            CSR_WRITE(BM_CSR_MITCMBASEADDR, value);
            break;
        case BM_CSR_MDTCMBASEADDR:
            CSR_WRITE(BM_CSR_MDTCMBASEADDR, value);
            break;
#endif
        default:
            bm_error("Unsupported CSR.");
    }
}

void target_csr_set_mask(const int csr, const xlen_t mask)
{
    switch (csr)
    {
        case BM_CSR_SBPREDCTRL:
            CSR_SET(BM_CSR_SBPREDCTRL, mask);
            break;
        case BM_CSR_MCFGCTRL:
            CSR_SET(BM_CSR_MCFGCTRL, mask);
            break;
        case BM_CSR_MDCACHECTRL:
            CSR_SET(BM_CSR_MDCACHECTRL, mask);
            break;
        case BM_CSR_MICACHECTRL:
            CSR_SET(BM_CSR_MICACHECTRL, mask);
            break;
        case BM_CSR_ML2CACHECTRL:
            CSR_SET(BM_CSR_ML2CACHECTRL, mask);
            break;
        case BM_CSR_MTLBCTRL:
            CSR_SET(BM_CSR_MTLBCTRL, mask);
            break;
        case BM_CSR_MCACHESTATUS:
            CSR_SET(BM_CSR_MCACHESTATUS, mask);
            break;
#ifdef CONFIG_HAS_TCM
        case BM_CSR_MTCMCFG:
            CSR_SET(BM_CSR_MTCMCFG, mask);
            break;
        case BM_CSR_MITCMBASEADDR:
            CSR_SET(BM_CSR_MITCMBASEADDR, mask);
            break;
        case BM_CSR_MDTCMBASEADDR:
            CSR_SET(BM_CSR_MDTCMBASEADDR, mask);
            break;
#endif
        default:
            bm_error("Unsupported CSR.");
    }
}

void target_csr_clear_mask(const int csr, const xlen_t mask)
{
    switch (csr)
    {
        case BM_CSR_SBPREDCTRL:
            CSR_CLEAR(BM_CSR_SBPREDCTRL, mask);
            break;
        case BM_CSR_MCFGCTRL:
            CSR_CLEAR(BM_CSR_MCFGCTRL, mask);
            break;
        case BM_CSR_MDCACHECTRL:
            CSR_CLEAR(BM_CSR_MDCACHECTRL, mask);
            break;
        case BM_CSR_MICACHECTRL:
            CSR_CLEAR(BM_CSR_MICACHECTRL, mask);
            break;
        case BM_CSR_ML2CACHECTRL:
            CSR_CLEAR(BM_CSR_ML2CACHECTRL, mask);
            break;
        case BM_CSR_MTLBCTRL:
            CSR_CLEAR(BM_CSR_MTLBCTRL, mask);
            break;
        case BM_CSR_MCACHESTATUS:
            CSR_CLEAR(BM_CSR_MCACHESTATUS, mask);
            break;
#ifdef CONFIG_HAS_TCM
        case BM_CSR_MTCMCFG:
            CSR_CLEAR(BM_CSR_MTCMCFG, mask);
            break;
        case BM_CSR_MITCMBASEADDR:
            CSR_CLEAR(BM_CSR_MITCMBASEADDR, mask);
            break;
        case BM_CSR_MDTCMBASEADDR:
            CSR_CLEAR(BM_CSR_MDTCMBASEADDR, mask);
            break;
#endif
        default:
            bm_error("Unsupported CSR.");
    }
}

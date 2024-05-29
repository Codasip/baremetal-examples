/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/verbose.h"

xlen_t target_csr_read(const int csr)
{
    xlen_t value = 0;

    switch (csr)
    {
        case BM_CSR_MEXCAUSE:
            CSR_READ(BM_CSR_MEXCAUSE, value);
            break;
#ifdef TARGET_HAS_CACHE
        case BM_CSR_ML1CACHE_START:
            CSR_READ(BM_CSR_ML1CACHE_START, value);
            break;
#endif
#ifdef TARGET_HAS_TCM
        case BM_CSR_MITCMEN:
            CSR_READ(BM_CSR_MITCMEN, value);
            break;
        case BM_CSR_MDTCMEN:
            CSR_READ(BM_CSR_MDTCMEN, value);
            break;
        case BM_CSR_MITCMSIZE:
            CSR_READ(BM_CSR_MITCMSIZE, value);
            break;
        case BM_CSR_MITCMADDR:
            CSR_READ(BM_CSR_MITCMADDR, value);
            break;
        case BM_CSR_MDTCMSIZE:
            CSR_READ(BM_CSR_MDTCMSIZE, value);
            break;
        case BM_CSR_MDTCMADDR:
            CSR_READ(BM_CSR_MDTCMADDR, value);
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
        case BM_CSR_MEXCAUSE:
            CSR_WRITE(BM_CSR_MEXCAUSE, value);
            break;
#ifdef TARGET_HAS_CACHE
        case BM_CSR_ML1CACHE_START:
            CSR_WRITE(BM_CSR_ML1CACHE_START, value);
            break;
#endif
#ifdef TARGET_HAS_TCM
        case BM_CSR_MITCMEN:
            CSR_WRITE(BM_CSR_MITCMEN, value);
            break;
        case BM_CSR_MDTCMEN:
            CSR_WRITE(BM_CSR_MDTCMEN, value);
            break;
        case BM_CSR_MITCMSIZE:
            CSR_WRITE(BM_CSR_MITCMSIZE, value);
            break;
        case BM_CSR_MITCMADDR:
            CSR_WRITE(BM_CSR_MITCMADDR, value);
            break;
        case BM_CSR_MDTCMSIZE:
            CSR_WRITE(BM_CSR_MDTCMSIZE, value);
            break;
        case BM_CSR_MDTCMADDR:
            CSR_WRITE(BM_CSR_MDTCMADDR, value);
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
        case BM_CSR_MEXCAUSE:
            CSR_SET(BM_CSR_MEXCAUSE, mask);
            break;
#ifdef TARGET_HAS_CACHE
        case BM_CSR_ML1CACHE_START:
            CSR_SET(BM_CSR_ML1CACHE_START, mask);
            break;
#endif
#ifdef TARGET_HAS_TCM
        case BM_CSR_MITCMEN:
            CSR_SET(BM_CSR_MITCMEN, mask);
            break;
        case BM_CSR_MDTCMEN:
            CSR_SET(BM_CSR_MDTCMEN, mask);
            break;
        case BM_CSR_MITCMSIZE:
            CSR_SET(BM_CSR_MITCMSIZE, mask);
            break;
        case BM_CSR_MITCMADDR:
            CSR_SET(BM_CSR_MITCMADDR, mask);
            break;
        case BM_CSR_MDTCMSIZE:
            CSR_SET(BM_CSR_MDTCMSIZE, mask);
            break;
        case BM_CSR_MDTCMADDR:
            CSR_SET(BM_CSR_MDTCMADDR, mask);
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
        case BM_CSR_MEXCAUSE:
            CSR_CLEAR(BM_CSR_MEXCAUSE, mask);
            break;
#ifdef TARGET_HAS_CACHE
        case BM_CSR_ML1CACHE_START:
            CSR_CLEAR(BM_CSR_ML1CACHE_START, mask);
            break;
#endif
#ifdef TARGET_HAS_TCM
        case BM_CSR_MITCMEN:
            CSR_CLEAR(BM_CSR_MITCMEN, mask);
            break;
        case BM_CSR_MDTCMEN:
            CSR_CLEAR(BM_CSR_MDTCMEN, mask);
            break;
        case BM_CSR_MITCMSIZE:
            CSR_CLEAR(BM_CSR_MITCMSIZE, mask);
            break;
        case BM_CSR_MITCMADDR:
            CSR_CLEAR(BM_CSR_MITCMADDR, mask);
            break;
        case BM_CSR_MDTCMSIZE:
            CSR_CLEAR(BM_CSR_MDTCMSIZE, mask);
            break;
        case BM_CSR_MDTCMADDR:
            CSR_CLEAR(BM_CSR_MDTCMADDR, mask);
            break;
#endif
        default:
            bm_error("Unsupported CSR.");
    }
}

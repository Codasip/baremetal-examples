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
        case BM_CSR_MEXCAUSE:
            CSR_READ(BM_CSR_MEXCAUSE, value);
            break;
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
        default:
            bm_error("Unsupported CSR.");
    }
}

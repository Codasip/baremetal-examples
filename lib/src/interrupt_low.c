/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/interrupt_low.h"

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/priv.h"
#include "baremetal/verbose.h"

#ifdef TARGET_HAS_CLIC
    #include "baremetal/clic.h"
#endif

#include <stdint.h>

void bm_interrupt_enable(bm_priv_mode_t priv_mode)
{
    bm_csr_id xstatus = bm_priv_get_csr_id(priv_mode, BM_PRIV_CSR_XSTATUS);
    bm_csr_set_mask(xstatus, 1 << priv_mode);
}

void bm_interrupt_disable(bm_priv_mode_t priv_mode)
{
    bm_csr_id xstatus = bm_priv_get_csr_id(priv_mode, BM_PRIV_CSR_XSTATUS);
    bm_csr_clear_mask(xstatus, 1 << priv_mode);
}

void bm_interrupt_enable_source(bm_priv_mode_t priv_mode, bm_interrupt_source_t source)
{
    bm_csr_id xie = bm_priv_get_csr_id(priv_mode, BM_PRIV_CSR_XIE);
    bm_csr_set_mask(xie, 1 << source);
}

void bm_interrupt_disable_source(bm_priv_mode_t priv_mode, bm_interrupt_source_t source)
{
    bm_csr_id xie = bm_priv_get_csr_id(priv_mode, BM_PRIV_CSR_XIE);
    bm_csr_clear_mask(xie, 1 << source);
}

#if defined(TARGET_EXT_N) || defined(TARGET_EXT_S)
void bm_interrupt_delegate_source(bm_priv_mode_t priv_mode, bm_interrupt_source_t source)
{
    if (priv_mode >= bm_get_priv_mode())
    {
        bm_error("Can only delegate to a lower mode.");
    }

    switch (priv_mode)
    {
    #ifdef TARGET_EXT_S
        case BM_PRIV_MODE_SUPERVISOR:
            bm_csr_set_mask(BM_CSR_MIDELEG, 1 << source);
            break;
    #endif
    #ifdef TARGET_EXT_N
        case BM_PRIV_MODE_USER:
            if (bm_get_priv_mode() == BM_PRIV_MODE_MACHINE)
            {
                bm_csr_set_mask(BM_CSR_MIDELEG, 1 << source);
            }
        #ifdef TARGET_EXT_S
            bm_csr_set_mask(BM_CSR_SIDELEG, 1 << source);
        #endif
            break;
    #endif
        default:
            bm_error("Unsupported privilege mode.");
    }
}

void bm_exception_delegate_source(bm_priv_mode_t priv_mode, bm_exception_source_t exception)
{
    if (priv_mode >= bm_get_priv_mode())
    {
        bm_error("Can only delegate to a lower mode.");
    }

    switch (priv_mode)
    {
    #ifdef TARGET_EXT_S
        case BM_PRIV_MODE_SUPERVISOR:
            bm_csr_set_mask(BM_CSR_MEDELEG, 1 << exception);
            break;
    #endif
    #ifdef TARGET_EXT_N
        case BM_PRIV_MODE_USER:
            if (bm_get_priv_mode() == BM_PRIV_MODE_MACHINE)
            {
                bm_csr_set_mask(BM_CSR_MEDELEG, 1 << exception);
            }
        #ifdef TARGET_EXT_S
            bm_csr_set_mask(BM_CSR_SEDELEG, 1 << exception);
        #endif
            break;
    #endif
        default:
            bm_error("Unsupported privilege mode.");
    }
}
#endif

xlen_t bm_interrupt_tvec_get_address(bm_priv_mode_t priv_mode)
{
    bm_csr_id tvec = bm_priv_get_csr_id(priv_mode, BM_PRIV_CSR_XTVEC);
    return bm_csr_read(tvec) & ~(xlen_t)0x3;
}

bm_interrupt_mode_t bm_interrupt_tvec_get_mode(bm_priv_mode_t priv_mode)
{
    bm_csr_id tvec = bm_priv_get_csr_id(priv_mode, BM_PRIV_CSR_XTVEC);
    return (tvec & 1) ? BM_INTERRUPT_MODE_VECTOR : BM_INTERRUPT_MODE_DIRECT;
}

void bm_interrupt_tvec_setup(bm_priv_mode_t priv_mode, xlen_t address, bm_interrupt_mode_t mode)
{
    const xlen_t value = (mode == BM_INTERRUPT_MODE_DIRECT) ? 0 : 1;

    bm_csr_id tvec = bm_priv_get_csr_id(priv_mode, BM_PRIV_CSR_XTVEC);
    bm_csr_write(tvec, (address & ~(xlen_t)0x3) | value);
}

void bm_wfi(void)
{
    __asm__ volatile("wfi");
}

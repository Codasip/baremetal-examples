/* Copyright 2023-2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_TARGET_TCM_H
#define BAREMETAL_TARGET_TCM_H

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/mem_barrier.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BM_PLACE_IN_ITCM __attribute__((section(".itcm")))
#define BM_PLACE_IN_DTCM __attribute__((section(".dtcm")))

/**
 * \brief Return address of the ITCM
 *
 * \return Address of ITCM
 */
xlen_t bm_tcm_itcm_get_base_address(void)
{
    return bm_csr_read(BM_CSR_MITCMBASEADDR);
}

/**
* \brief Return address of the DTCM
 *
* \return Address of DTCM
*/
xlen_t bm_tcm_dtcm_get_base_address(void)
{
    return bm_csr_read(BM_CSR_MDTCMBASEADDR);
}

/**
 * \brief Enable ITCM
 */
void bm_tcm_itcm_enable(void)
{
    bm_csr_set_mask(BM_CSR_MTCMCFG, BM_CSR_MTCMCFG__ITCM_ENABLE_MASK);
}

/**
 * \brief Enable DTCM
 */
void bm_tcm_dtcm_enable(void)
{
    bm_csr_set_mask(BM_CSR_MTCMCFG, BM_CSR_MTCMCFG__DTCM_ENABLE_MASK);
}

/**
 * \brief Disable ITCM
 */
void bm_tcm_itcm_disable(void)
{
    bm_csr_clear_mask(BM_CSR_MTCMCFG, BM_CSR_MTCMCFG__ITCM_ENABLE_MASK);
}

/**
 * \brief Disable DTCM
 */
void bm_tcm_dtcm_disable(void)
{
    bm_csr_clear_mask(BM_CSR_MTCMCFG, BM_CSR_MTCMCFG__DTCM_ENABLE_MASK);
}

/**
 * \brief Get size of ITCM
 *
 * \return Size of ITCM
 */
xlen_t bm_tcm_itcm_get_size(void)
{
    bm_csr_mtcmcfg_t mtcmcfg;

    mtcmcfg.reg = bm_csr_read(BM_CSR_MTCMCFG);

    return mtcmcfg.itcm_size * 1024;
}

/**
 * \brief Get size of DTCM
 *
 * \return Size of DTCM
 */
xlen_t bm_tcm_dtcm_get_size(void)
{
    bm_csr_mtcmcfg_t mtcmcfg;

    mtcmcfg.reg = bm_csr_read(BM_CSR_MTCMCFG);

    return mtcmcfg.dtcm_size * 1024;
}

// Symbols defined in the linker script
extern char __itcm_data_start;
extern char __dtcm_data_start;
extern char __itcm_data_end;
extern char __dtcm_data_end;

/**
 * \brief Copy contents of the .itcm section to the instruction TCM
 */
void bm_tcm_itcm_init(void)
{
    xlen_t itcm_start    = bm_tcm_itcm_get_base_address();
    void  *itcm_data     = &__itcm_data_start;
    void  *itcm_data_end = &__itcm_data_end;

    memcpy((void *)itcm_start, itcm_data, ((xlen_t)itcm_data_end - (xlen_t)itcm_data));

    bm_exec_fence();
    bm_exec_fence_i();
}

/**
 * \brief Copy contents of the .dtcm section to the data TCM
 */
void bm_tcm_dtcm_init(void)
{
    xlen_t dtcm_start    = bm_tcm_dtcm_get_base_address();
    void  *dtcm_data     = &__dtcm_data_start;
    void  *dtcm_data_end = &__dtcm_data_end;

    memcpy((void *)dtcm_start, dtcm_data, ((xlen_t)dtcm_data_end - (xlen_t)dtcm_data));

    bm_exec_fence();
}

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_TARGET_TCM_H */

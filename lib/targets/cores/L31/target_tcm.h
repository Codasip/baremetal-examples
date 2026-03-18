/* Copyright 2023-2025 Codasip s.r.o.    */
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
static inline xlen_t bm_tcm_itcm_get_base_address(void)
{
    xlen_t val = 0;
    BM_CSR_READ(BM_CSR_MITCMADDR, val);
    return val;
}

/**
* \brief Return address of the DTCM
 *
* \return Address of DTCM
*/
static inline xlen_t bm_tcm_dtcm_get_base_address(void)
{
    xlen_t val = 0;
    BM_CSR_READ(BM_CSR_MDTCMADDR, val);
    return val;
}

/**
 * \brief Enable ITCM
 */
static inline void bm_tcm_itcm_enable(void)
{
    BM_CSR_WRITE(BM_CSR_MITCMEN, 1);
}

/**
 * \brief Enable DTCM
 */
static inline void bm_tcm_dtcm_enable(void)
{
    BM_CSR_WRITE(BM_CSR_MDTCMEN, 1);
}

/**
 * \brief Disable ITCM
 */
static inline void bm_tcm_itcm_disable(void)
{
    BM_CSR_WRITE(BM_CSR_MITCMEN, 0);
}

/**
 * \brief Disable DTCM
 */
static inline void bm_tcm_dtcm_disable(void)
{
    BM_CSR_WRITE(BM_CSR_MDTCMEN, 0);
}

/**
 * \brief Get size of ITCM
 *
 * \return Size of ITCM
 */
static inline xlen_t bm_tcm_itcm_get_size(void)
{
    xlen_t val = 0;
    BM_CSR_READ(BM_CSR_MITCMSIZE, val);
    return val;
}

/**
 * \brief Get size of DTCM
 *
 * \return Size of DTCM
 */
static inline xlen_t bm_tcm_dtcm_get_size(void)
{
    xlen_t val = 0;
    BM_CSR_READ(BM_CSR_MDTCMSIZE, val);
    return val;
}

// Symbols defined in the linker script
extern char __itcm_data_start;
extern char __dtcm_data_start;
extern char __itcm_data_end;
extern char __dtcm_data_end;

/**
 * \brief Copy contents of the .itcm section to the instruction TCM
 */
static inline void bm_tcm_itcm_init(void)
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
static inline void bm_tcm_dtcm_init(void)
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

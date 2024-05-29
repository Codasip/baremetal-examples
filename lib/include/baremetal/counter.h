/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_COUNTER_H
#define BAREMETAL_COUNTER_H

#include "baremetal/priv.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Enumeration of all counters available on the target */
typedef enum {
    BM_COUNTER_CYCLE,
    BM_COUNTER_TIME,
    BM_COUNTER_INSTRET,
    BM_COUNTER_HPM3,
    BM_COUNTER_HPM4,
    BM_COUNTER_HPM5,
    BM_COUNTER_HPM6,
    BM_COUNTER_HPM7,
    BM_COUNTER_HPM8,
    BM_COUNTER_HPM9,
    BM_COUNTER_HPM10,
    BM_COUNTER_HPM11,
    BM_COUNTER_HPM12,
    BM_COUNTER_HPM13,
    BM_COUNTER_HPM14,
    BM_COUNTER_HPM15,
    BM_COUNTER_HPM16,
    BM_COUNTER_HPM17,
    BM_COUNTER_HPM18,
    BM_COUNTER_HPM19,
    BM_COUNTER_HPM20,
    BM_COUNTER_HPM21,
    BM_COUNTER_HPM22,
    BM_COUNTER_HPM23,
    BM_COUNTER_HPM24,
    BM_COUNTER_HPM25,
    BM_COUNTER_HPM26,
    BM_COUNTER_HPM27,
    BM_COUNTER_HPM28,
    BM_COUNTER_HPM29,
    BM_COUNTER_HPM30,
    BM_COUNTER_HPM31,
} bm_counter_id;

/**
 * \brief Obtain value from a counter
 *
 * \param counter Counter to read
 */
uint64_t bm_counter_read(bm_counter_id counter);

/**
 * \brief Clear given counter
 *
 * \param counter Counter to clear
 */
void bm_counter_clear(bm_counter_id counter);

#if defined(TARGET_EXT_U) || defined(TARGET_EXT_S)
/**
 * \brief Enable a counter for the given privilege mode
 *
 * \param priv_mode Privilege mode
 * \param counter Counter to enable
 */
void bm_counter_enable_priv(bm_priv_mode_t priv_mode, bm_counter_id counter);
#endif

/**
 * \brief Pause given counter
 *
 * \param counter Counter to pause
 */
void bm_counter_pause(bm_counter_id counter);

/**
 * \brief Resume given counter
 *
 * \param counter Counter to resume
 */
void bm_counter_resume(bm_counter_id counter);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_COUNTER_H */

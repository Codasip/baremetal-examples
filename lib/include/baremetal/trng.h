/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_TRNG_H
#define BAREMETAL_TRNG_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Structure describing TRNG peripheral registers */
typedef struct bm_trng_regs bm_trng_regs_t;

/** \brief Structure holding data neccessary to service the TRNG peripheral. */
typedef struct {
    bm_trng_regs_t *regs; ///< Pointer to the peripheral registers
} bm_trng_t;

/**
 * \brief Configuration parameters for the TRNG
 *
 * The TRNG contains a counter; configuration specifies at which values of the counter
 * should the TEROs be sampled and cleared and when it should wrap
 */
typedef struct bm_trng_config {
    uint8_t counter_sample_tero; ///< Sample TEROs at this count
    uint8_t counter_clear_tero;  ///< Clear TEROs at this count
    uint8_t counter_wrap;        ///< Wrap counter at this count
    bool    checking;            ///< Enable error checking
    bool    unbiasing;           ///< Enable unbiasing
} bm_trng_config_t;

/**
 * \brief Retrieve a raw data sample
 *
 * \param trng TRNG peripheral
 *
 * \return Raw data
 */
uint32_t bm_trng_get_raw(bm_trng_t *trng);

/**
 * \brief Retrieve a processed, random, data sample
 *
 * \param trng TRNG peripheral
 *
 * \return Random data
 */
uint32_t bm_trng_get_rnd(bm_trng_t *trng);

/**
 * \brief Read TRNG status
 *
 * \param trng TRNG peripheral
 *
 * \return Number of errors
 */
uint32_t bm_trng_get_status(bm_trng_t *trng);

/**
 * \brief Configure TRNG peripheral
 *
 * \param trng TRNG peripheral
 * \param config TRNG configuration
 */
void bm_trng_configure(bm_trng_t *trng, const bm_trng_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_TRNG_H */

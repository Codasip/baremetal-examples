/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_CLIC_H
#define BAREMETAL_CLIC_H

#include "baremetal/interrupt_low.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Structure describing CLIC peripheral registers */
typedef struct bm_clic_regs bm_clic_regs_t;

/** \brief Structure holding data neccessary to service the CLIC peripheral. */
typedef struct {
    struct bm_clic_regs *regs; ///< Pointer to the peripheral registers
} bm_clic_t;

/**
 * \brief Convert interrupt source to CLIC interrupt ID
 *
 * \param irq Interrupt source
 *
 * \return CLIC interrupt ID for the given source
 */
unsigned bm_clic_get_irq_id(bm_interrupt_source_t irq);

/**
 * \brief Convert external interrupt ID to CLIC interrupt ID
 *
 * \param ext_irq_id External interrupt ID
 *
 * \return CLIC interrupt ID for the given source
 */
unsigned bm_clic_get_ext_irq_id(unsigned ext_irq_id);

/**
 * \brief Initialize CLIC registers
 *
 * \param clic CLIC device
 */
void bm_clic_init(bm_clic_t *clic);

/**
 * \brief Enable or disable given CLIC interrupt ID
 *
 * \param clic CLIC device
 * \param clic_irq_id CLIC interrupt ID to enable
 * \param en Value to set (enable/disable)
 */
void bm_clic_set_enable(bm_clic_t *clic, unsigned clic_irq_id, bool en);

/**
 * \brief Find whether given CLIC interrupt ID is enabled
 *
 * \param clic CLIC device
 * \param clic_irq_id CLIC interrupt ID to check
 *
 * \return True if the CLIC interrupt ID is enabled otherwise False
 */
bool bm_clic_get_enable(bm_clic_t *clic, unsigned clic_irq_id);

/**
 * \brief Set level for given CLIC interrupt ID
 *
 * \param clic CLIC device
 * \param clic_irq_id CLIC interrupt ID to configure
 * \param level Interrupt level to set
 */
void bm_clic_set_level(bm_clic_t *clic, unsigned clic_irq_id, uint8_t level);

/**
 * \brief Get interrupt level for given CLIC interrupt ID
 *
 * \param clic CLIC device
 * \param clic_irq_id CLIC interrupt ID to check
 *
 * \return Interrupt level for the given CLIC interrupt ID
 */
uint8_t bm_clic_get_level(bm_clic_t *clic, unsigned clic_irq_id);

/**
 * \brief Enable or disable hardware vectoring for given CLIC interrupt ID
 *
 * \param clic CLIC device
 * \param clic_irq_id CLIC interrupt ID to configure
 * \param shv Value to set (enable/disable hardware vectoring)
 */
void bm_clic_set_vectored(bm_clic_t *clic, unsigned clic_irq_id, bool shv);

/**
 * \brief Find whether interrupt is pending for given CLIC interrupt ID
 *
 * \param clic CLIC device
 * \param clic_irq_id CLIC interrupt ID to check
 *
 * \return True if interrupt is pending for the given CLIC interrup ID, otherwise False
 */
bool bm_clic_get_pending(bm_clic_t *clic, unsigned clic_irq_id);

#endif /* BAREMETAL_CLIC_H */

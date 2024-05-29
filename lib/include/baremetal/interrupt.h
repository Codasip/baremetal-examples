/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_INTERRUPT_H
#define BAREMETAL_INTERRUPT_H

#include "baremetal/csr.h"
#include "baremetal/interrupt_low.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Set function to handle given interrupt cause
 *
 * \param cause Interrupt source to handle
 * \param func Function to set as the handler
 */
void bm_interrupt_set_handler(bm_interrupt_source_t cause, void (*func)(void));

/**
 * \brief Set function to handle given exception cause
 *
 * \param cause Exception source to handle
 * \param func Function to set as the handler
 */
void bm_exception_set_handler(bm_exception_source_t cause, void (*func)(void));

/**
 * \brief Set function to handle given external interrupt source
 *
 * \param ext_irq_id External interrupt source ID
 * \param func Function to set as the handler
 */
void bm_ext_irq_set_handler(unsigned ext_irq_id, void (*func)(void));

/**
 * \brief Initialize interrupt handling for given privilege mode and interrupt or exceptions sources
 *
 * \param priv_mode Privilege mode to initialize the interrupt handling for
 */
void bm_interrupt_init(bm_priv_mode_t priv_mode);

/**
 * \brief Claim an external interrupt
 *
 * \return External interrupt identifier or -1
 */
int bm_ext_irq_claim(void);

/**
 * \brief Finalize handling of an external interrupt
 *
 * \param irq External interrupt identifier
 */
void bm_ext_irq_complete(unsigned ext_irq_id);

/**
 * \brief Enable external interrupt
 *
 * \param irq External interrupt identifier
 */
void bm_ext_irq_enable(unsigned ext_irq_id);

/**
 * \brief Disable external interrupt
 *
 * \param irq External interrupt identifier
 */
void bm_ext_irq_disable(unsigned ext_irq_id);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_INTERRUPT_H */

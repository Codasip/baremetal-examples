/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_CLINT_H
#define BAREMETAL_CLINT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Structure describing CLINT peripheral registers */
typedef struct bm_clint_regs bm_clint_regs_t;

/** \brief Structure holding data neccessary to service the CLINT peripheral. */
typedef struct {
    struct bm_clint_regs *regs; ///< Pointer to the peripheral registers
    unsigned              freq; ///< Clock frequency
} bm_clint_t;

/**
 * \brief Convert number of ticks to milliseconds
 *
 * \param clint CLINT device
 * \param ticks Ticks
 *
 * \return Time in milliseconds corresponding to the given number of ticks
 */
unsigned bm_clint_ticks_to_ms(const bm_clint_t *clint, uint64_t ticks);

/**
 * \brief Convert milliseconds to number of ticks
 *
 * \param clint CLINT device
 * \param milliseconds Milliseconds
 *
 * \return Number of ticks corresponding to the given time
 */
uint64_t bm_clint_ms_to_ticks(const bm_clint_t *clint, unsigned milliseconds);

/**
 * \brief Get current MTIME register value
 *
 * \param clint CLINT device
 *
 * \return Time in number of ticks
 */
uint64_t bm_clint_get_mtime(bm_clint_t *clint);

/**
 * \brief Set the MTIMECMP register for a specific hart to a specified time
 *
 * \param clint CLINT device
 * \param hart_id ID of the hart
 * \param time_value Time when to trigger timer interrupt
 */
void bm_clint_set_mtimecmp(bm_clint_t *clint, unsigned hart_id, uint64_t time_value);

/**
 * \brief Set a timer for a specific hart to trigger after a specific time
 *
 * \param clint CLINT device
 * \param hart_id ID of the hart
 * \param ticks_from_now How many ticks from now should the timer trigger
 */
void bm_clint_arm_timer(bm_clint_t *clint, unsigned hart_id, uint64_t ticks_from_now);

/**
 * \brief Re-arm the timer for a specific hart so that it triggers after a specific delay from the last trigger
 *
 * \param clint CLINT device
 * \param hart_id ID of the hart
 * \param delay_ticks Number of ticks between the triggers
 *
 * \note This function is expected to be called from an ISR when implementing a periodic timer.
 */
void bm_clint_rearm_timer(bm_clint_t *clint, unsigned hart_id, uint64_t delay_ticks);

/**
 * \brief Send an IPI (Inter-Processor Interrupt) to a specific hart
 *
 * \param clint CLINT device
 * \param hart_id ID of the target hart
 */
void bm_clint_send_ipi(bm_clint_t *clint, unsigned hart_id);

/**
 * \brief Clear IPI register of a specific hart
 *
 * \param clint CLINT device
 * \param hart_id ID of the target hart
 *
 * \note This function is expected to be called from an ISR to clear the interrupt.
 */
void bm_clint_clear_ipi(bm_clint_t *clint, unsigned hart_id);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_CLINT_H */

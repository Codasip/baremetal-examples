/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_TIME_H
#define BAREMETAL_TIME_H

#include "baremetal/counter.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get current value from timer.
 *
 * \return Number of cycles read from times
 */
static inline uint64_t bm_get_cycles(void)
{
    return bm_counter_read(BM_COUNTER_CYCLE);
}

/**
 * \brief Convert number of cycles to milliseconds
 *
 * \param cycles Cycles
 *
 * \return Time in milliseconds corresponding to the given number of cycles
 */
static inline unsigned bm_cycles_to_ms(uint64_t cycles)
{
    return cycles / (TARGET_CLK_FREQ / 1000);
}

/**
 * \brief Convert milliseconds to number of cycles
 *
 * \param milliseconds Milliseconds
 *
 * \return Number of cycles corresponding to the given time
 */
static inline uint64_t bm_ms_to_cycles(unsigned milliseconds)
{
    return (uint64_t)milliseconds * (TARGET_CLK_FREQ / 1000);
}

/**
 * \brief Get current value from timer in milliseconds.
 *
 * \return Milliseconds since the counter was last cleared
 */
static inline unsigned bm_get_time_ms(void)
{
    return bm_cycles_to_ms(bm_get_cycles());
}

/**
 * \brief Delay execution for a given amount of CPU cycles
 *
 * \param cycles Number of CPU cycles to wait
 */
static inline void bm_delay_cycles(uint64_t cycles)
{
    uint64_t current_cycles = bm_get_cycles();
    uint64_t end_cycles     = current_cycles + cycles;

    while (current_cycles < end_cycles)
    {
        current_cycles = bm_get_cycles();
    }
}

/**
 * \brief Delay execution for a given amount of time
 *
 * \param milliseconds Time (number of milliseconds) to wait
 */
static inline void bm_delay_ms(unsigned milliseconds)
{
    bm_delay_cycles(bm_ms_to_cycles(milliseconds));
}

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_TIME_H */

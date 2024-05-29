/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_MUTEX_H
#define BAREMETAL_MUTEX_H

#include "baremetal/common.h"

#include <stdint.h>

#if !defined(__riscv_atomic) && (TARGET_NUM_HARTS > 1)
    #error "Mutex functionality is not defined for this target"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Simple mutex for hart synchronization */
typedef volatile uint32_t bm_mutex_t;

/**
 * \brief Initialize given mutex
 *
 * \param mutex Mutex to initialize
 */
void bm_mutex_init(bm_mutex_t *mutex);

/**
 * \brief Try to claim a mutex for the current hart.
 *
 * \param mutex Mutex to try to claim
 *
 * \return 0 on success, non-zero otherwise
 */
int bm_mutex_trylock(bm_mutex_t *mutex);

/**
 * \brief Claim a mutex for the current hart.
 *
 * \param mutex Mutex to claim
 */
void bm_mutex_lock(bm_mutex_t *mutex);

/**
 * \brief Release claimed lock.
 *
 * \param mutex Mutex to release
 */
void bm_mutex_unlock(bm_mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_MUTEX_H */

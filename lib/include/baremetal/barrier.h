/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_BARRIER_H
#define BAREMETAL_BARRIER_H

#include "baremetal/common.h"

#include <stdint.h>

#if !defined(__riscv_atomic) && (TARGET_NUM_HARTS > 1)
    #error "Barrier functionality is not defined for this target"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Simple barrier for hart synchronization */
typedef struct {
    volatile uint32_t waiting;
    volatile uint32_t done;
} bm_barrier_t;

/**
 * \brief Initialize given barrier
 *
 * \param barrier Barrier to initialize
 */
void bm_barrier_init(bm_barrier_t *barrier);

/**
 * \brief Wait until all harts call this function
 *
 * \param barrier Barrier to synchronize harts by
 */
void bm_barrier_wait(bm_barrier_t *barrier);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_BARRIER_H */

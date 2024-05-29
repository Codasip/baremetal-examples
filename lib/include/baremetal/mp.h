/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_MP_H
#define BAREMETAL_MP_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *bm_hart_func_arg_t;
typedef void (*bm_hart_func_ptr_t)(bm_hart_func_arg_t);

/**
 * \brief Instruct hart to start executing a given function
 *
 * \param hart_id ID of the hart to start execution on
 * \param func Function to execute
 * \param arg Argument to pass to the executed function
 *
 * \return 0 on success, non-zero otherwise
 */
int bm_hart_start(unsigned hart_id, bm_hart_func_ptr_t func, bm_hart_func_arg_t arg);

/**
 * \brief Check whether specified hart currently executes a function
 *
 * \param hart_id ID of hart to check
 *
 * \return true if running, false if hart is available
 */
bool bm_hart_running(unsigned hart_id);

/**
 * \brief Wait until execution on specified hart finishes
 *
 * \param hart_id ID of hart to wait for
 */
void bm_hart_join(unsigned hart_id);

/**
 * \brief Execute a function from all harts in parallel
 *
 * \param func Function to execute
 */
void bm_hart_execute_all(bm_hart_func_ptr_t func);

/**
 * \brief Get hart ID
 *
 * \return ID of the current hart
 */
unsigned bm_get_hartid(void);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_MP_H */

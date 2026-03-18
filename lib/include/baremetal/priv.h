/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_PRIV_H
#define BAREMETAL_PRIV_H

#include "baremetal/common.h"
#include "baremetal/csr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Privilege levels */
typedef enum {
    BM_PRIV_MODE_USER       = 0,
    BM_PRIV_MODE_SUPERVISOR = 1,
    BM_PRIV_MODE_MACHINE    = 3,
} bm_priv_mode_t;

/** \brief Enumeration of CSR types available in different privilege modes */
typedef enum {
    BM_PRIV_CSR_XCAUSE,
    BM_PRIV_CSR_XEPC,
    BM_PRIV_CSR_XIE,
    BM_PRIV_CSR_XIP,
    BM_PRIV_CSR_XSTATUS,
    BM_PRIV_CSR_XTVAL,
    BM_PRIV_CSR_XTVEC,
} bm_csr_type_t;

extern volatile bm_priv_mode_t bm_current_mode;

/**
 * \brief Get the current privilege level
 */
bm_priv_mode_t bm_get_priv_mode(void);

/**
 * \brief Get CSR ID of given type in given privilege mode
 *
 * \param priv_mode Privilege mode
 * \param csr CSR type
 *
 * \return CSR ID
 */
bm_csr_id_t bm_priv_get_csr_id(bm_priv_mode_t priv_mode, bm_csr_type_t csr);

/**
 * \brief Reads the value from CSR registers using a macro resolved from the CSR name variable.
 *
 * This function converts the given CSR name (as a variable) into a macro,
 * and reads the value.
 *
 * \param priv_mode Privilege mode
 * \param csr CSR type
 *
 * \return Value of the CSR
 */
xlen_t bm_priv_csr_read(bm_priv_mode_t priv_mode, bm_csr_type_t csr);

/**
 * \brief Writes a value to a CSR register using a macro resolved from the CSR name variable.
 *
 * This function converts the given CSR name (as a variable) into a macro,
 * and writes the specified value to the corresponding CSR (Control and Status Register).
 *
 * \param priv_mode Privilege mode
 * \param csr CSR type
 * \param val Value to be written to the CSR register
 */
void bm_priv_csr_write(bm_priv_mode_t priv_mode, bm_csr_type_t csr, xlen_t val);

/**
 * \brief Sets bits in a specified CSR register using a bitmask.
 *
 * This function takes a CSR register identifier and applies a bitwise set operation
 * using the provided mask value. It uses predefined macros to access and modify the appropriate CSR.
 *
 * \param priv_mode Privilege mode
 * \param csr CSR type
 * \param mask Bitmask indicating which bits to set in the specified CSR
 */
void bm_priv_csr_set(bm_priv_mode_t priv_mode, bm_csr_type_t csr, xlen_t mask);

/**
 * \brief Clears bits in a specified CSR register using a bitmask.
 *
 * This function takes a CSR register identifier and applies a bitwise set operation
 * using the provided mask value. It uses predefined macros to access and modify the appropriate CSR.
 *
 * \param priv_mode Privilege mode
 * \param csr CSR type
 * \param mask Bitmask indicating which bits to clear in the specified CSR
 */
void bm_priv_csr_clear(bm_priv_mode_t priv_mode, bm_csr_type_t csr, xlen_t mask);

/**
 * \brief Enter a lower privilege level
 *
 * \param mode Privilege level to drop the privilege to
 * \param entry Address to start executing at from the lower level
 * \param stack Address to initialize the stack pointer for the lover level
 */
typedef void (*entry_fn_t)(void);

void bm_priv_enter_mode(bm_priv_mode_t mode, entry_fn_t entry, uint8_t *stack);

/** \brief Syscall arguments */
typedef struct {
    xlen_t arg0;
    xlen_t arg1;
    xlen_t arg2;
    xlen_t arg3;
    xlen_t arg4;
    xlen_t arg5;
    xlen_t arg6;
    xlen_t arg7;
} bm_priv_call_args;

/**
 * \brief Execute syscall parametrized by the given arguments
 *
 * \param args Parameters of the call
 *
 * \return Call return value
 */
xlen_t bm_priv_execute_call(bm_priv_call_args args);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_PRIV_H */

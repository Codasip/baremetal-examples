/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_INTERRUPT_LOW_H
#define BAREMETAL_INTERRUPT_LOW_H

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/priv.h"

/** \brief Interrupt sources. */
typedef enum {
    BM_INTERRUPT_USIP = 0,  // User software interrupt
    BM_INTERRUPT_SSIP = 1,  // Supervisor software interrupt
    BM_INTERRUPT_MSIP = 3,  // Machine software interrupt
    BM_INTERRUPT_UTIP = 4,  // User timer interrupt
    BM_INTERRUPT_STIP = 5,  // Supervisor timer interrupt
    BM_INTERRUPT_MTIP = 7,  // Machine timer interrupt
    BM_INTERRUPT_UEIP = 8,  // User external interrupt
    BM_INTERRUPT_SEIP = 9,  // Supervisor external interrupt
    BM_INTERRUPT_MEIP = 11, // Machine external interrupt
} bm_interrupt_source_t;

/** \brief Exception sources */
typedef enum {
    BM_EXCEPTION_IAM     = 0,  // Instruction address misaligned
    BM_EXCEPTION_IAF     = 1,  // Instruction access fault
    BM_EXCEPTION_II      = 2,  // Illegal instruction
    BM_EXCEPTION_BREAK   = 3,  // Breakpoint
    BM_EXCEPTION_LAM     = 4,  // Load address misaligned
    BM_EXCEPTION_LAF     = 5,  // Load access fault
    BM_EXCEPTION_SAM     = 6,  // Store/AMO address misaligned
    BM_EXCEPTION_SAF     = 7,  // Store/AMO access fault
    BM_EXCEPTION_ECALL_U = 8,  // Environment call from U-mode
    BM_EXCEPTION_ECALL_S = 9,  // Environment call from S-mode
    BM_EXCEPTION_ECALL_M = 11, // Environment call from M-mode
    BM_EXCEPTION_IPF     = 12, // Instruction page fault
    BM_EXCEPTION_LPF     = 13, // Load page fault
    BM_EXCEPTION_SPF     = 15, // Store/AMO page fault
} bm_exception_source_t;

/** \brief Modes of trap handling. */
typedef enum {
    BM_INTERRUPT_MODE_DIRECT,
    BM_INTERRUPT_MODE_VECTOR,
} bm_interrupt_mode_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Enable all interrupts for given privilege mode
 *
 * \param priv_mode Privilege mode to enable the interrupts for
 */
void bm_interrupt_enable(bm_priv_mode_t priv_mode);

/**
 * \brief Disable all interrupts for given privilege mode
 *
 * \param priv_mode Privilege mode to disable the interrupts for
 */
void bm_interrupt_disable(bm_priv_mode_t priv_mode);

/**
 * \brief Enable interrupt source for the given privilege mode
 *
 * \param priv_mode Privilege mode
 * \param source Interrupt source to enable
 */
void bm_interrupt_enable_source(bm_priv_mode_t priv_mode, bm_interrupt_source_t source);

/**
 * \brief Disable interrupt source for the given privilege mode
 *
 * \param priv_mode Privilege mode
 * \param source Interrupt source to disable
 */
void bm_interrupt_disable_source(bm_priv_mode_t priv_mode, bm_interrupt_source_t source);

#if defined(TARGET_EXT_N) || defined(TARGET_EXT_S)
/**
 * \brief Delegate handling of interrupt source to lower privilege mode
 *
 * \param priv_mode Privilege mode to delegate to
 * \param source Interrupt source to delegate
 */
void bm_interrupt_delegate_source(bm_priv_mode_t priv_mode, bm_interrupt_source_t source);

/**
 * \brief Delegate handling of exception source to lower privilege mode
 *
 * \param priv_mode Privilege mode to delegate to
 * \param exceptions Exception source to delegate
 */
void bm_exception_delegate_source(bm_priv_mode_t priv_mode, bm_exception_source_t exception);
#endif

/**
 * \brief Return the address of the trap handler in the given privilege mode
 *
 * \param priv_mode Privilege mode
 *
 * \return Address of the trap handler
 */
xlen_t bm_interrupt_tvec_get_address(bm_priv_mode_t priv_mode);

/**
 * \brief Get trap handler mode in the given privilege mode
 *
 * \param priv_mode Privilege mode
 *
 * \return mode
 */
bm_interrupt_mode_t bm_interrupt_tvec_get_mode(bm_priv_mode_t priv_mode);

/**
 * \brief Set the address and mode of the trap handler in the given privilege mode
 *
 * \param priv_mode Privilege mode
 * \param address Address of the trap handler
 * \param mode Mode to set
 */
void bm_interrupt_tvec_setup(bm_priv_mode_t priv_mode, xlen_t address, bm_interrupt_mode_t mode);

/**
 * \brief Enter WFI (wait for interrupt) mode
 */
void bm_wfi();

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_INTERRUPT_LOW_H */

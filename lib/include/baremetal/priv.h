/* Copyright 2023 Codasip s.r.o.         */
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

/** \brief Register file */
typedef struct {
    xlen_t ra;
    xlen_t sp;
    xlen_t gp;
    xlen_t tp;
    xlen_t t0;
    xlen_t t1;
    xlen_t t2;
    xlen_t s0;
    xlen_t s1;
    xlen_t a0;
    xlen_t a1;
    xlen_t a2;
    xlen_t a3;
    xlen_t a4;
    xlen_t a5;
#ifndef __riscv_32e
    xlen_t a6;
    xlen_t a7;
    xlen_t s2;
    xlen_t s3;
    xlen_t s4;
    xlen_t s5;
    xlen_t s6;
    xlen_t s7;
    xlen_t s8;
    xlen_t s9;
    xlen_t s10;
    xlen_t s11;
    xlen_t t3;
    xlen_t t4;
    xlen_t t5;
    xlen_t t6;
#endif
#ifdef __riscv_flen
    xlen_t ft0;
    xlen_t ft1;
    xlen_t ft2;
    xlen_t ft3;
    xlen_t ft4;
    xlen_t ft5;
    xlen_t ft6;
    xlen_t ft7;
    xlen_t fs0;
    xlen_t fs1;
    xlen_t fa0;
    xlen_t fa1;
    xlen_t fa2;
    xlen_t fa3;
    xlen_t fa4;
    xlen_t fa5;
    xlen_t fa6;
    xlen_t fa7;
    xlen_t fs2;
    xlen_t fs3;
    xlen_t fs4;
    xlen_t fs5;
    xlen_t fs6;
    xlen_t fs7;
    xlen_t fs8;
    xlen_t fs9;
    xlen_t fs10;
    xlen_t fs11;
    xlen_t ft8;
    xlen_t ft9;
    xlen_t ft10;
    xlen_t ft11;
#endif
} bm_register_file_t;

extern volatile bm_priv_mode_t     bm_current_mode;
extern volatile bm_register_file_t bm_priv_regs[4];
extern volatile xlen_t             bm_priv_sp[4];

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
bm_csr_id bm_priv_get_csr_id(bm_priv_mode_t priv_mode, bm_csr_type_t csr);

/**
 * \brief Enter a lower privilege level
 *
 * \param mode Privilege level to drop the privilege to
 * \param entry Address to start executing at from the lower level
 * \param stack Address to initialize the stack pointer for the lover level
 */
void bm_priv_enter_mode(bm_priv_mode_t mode, xlen_t entry, xlen_t stack);

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

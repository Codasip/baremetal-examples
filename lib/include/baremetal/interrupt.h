/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_INTERRUPT_H
#define BAREMETAL_INTERRUPT_H

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/interrupt_low.h"

/* RISC-V rv32i Calling Convention, see https://riscv.org/wp-content/uploads/2024/12/riscv-calling.pdf
 *
 * Register ABI Name Description                        Saver   Save only if task swap
 * -------- -------- -----------                        -----   ----------------------
 * x0       zero     Hard-wired zero                    —
 * x1       ra       Return address                     Caller
 * x2       sp       Stack pointer                      Callee  Yes
 * x3       gp       Global pointer                     —
 * x4       tp       Thread pointer                     —
 * x5–7     t0–2     Temporaries                        Caller
 * x8       s0/fp    Saved register/frame pointer       Callee  Yes
 * x9       s1       Saved register                     Callee  Yes
 * x10–11   a0–1     Function arguments/return values   Caller
 * x12–17   a2–7     Function arguments                 Caller
 * x18–27   s2–11    Saved registers                    Callee  Yes
 * x28–31   t3–6     Temporaries                        Caller
 *
 * Floating Point context save/restore
 * f0–7     ft0–7    FP temporaries                     Caller
 * f8–9     fs0–1    FP saved registers                 Callee  Yes
 * f10–11   fa0–1    FP arguments/return values         Caller
 * f12–17   fa2–7    FP arguments                       Caller
 * f18–27   fs2–11   FP saved registers                 Callee  Yes
 * f28–31   ft8–11   FP temporaries                     Caller
 */

/** \brief Register file */
typedef struct {
    uintptr_t ra; // x1
    uintptr_t t0; // x5
    uintptr_t t1; // x6
    uintptr_t t2; // x7
    uintptr_t a0; // x10
    uintptr_t a1; // x11
    uintptr_t a2; // x12
    uintptr_t a3; // x13
    uintptr_t a4; // x14
    uintptr_t a5; // x15
#ifndef __riscv_32e
    uintptr_t a6; // x16
    uintptr_t a7; // x17
    uintptr_t t3; // x28
    uintptr_t t4; // x29
    uintptr_t t5; // x30
    uintptr_t t6; // x31
#endif

#if __riscv_flen
    flen_t ft0;  // f0
    flen_t ft1;  // f1
    flen_t ft2;  // f2
    flen_t ft3;  // f3
    flen_t ft4;  // f4
    flen_t ft5;  // f5
    flen_t ft6;  // f6
    flen_t ft7;  // f7
    flen_t fa0;  // f10
    flen_t fa1;  // f11
    flen_t fa2;  // f12
    flen_t fa3;  // f13
    flen_t fa4;  // f14
    flen_t fa5;  // f15
    flen_t fa6;  // f16
    flen_t fa7;  // f17
    flen_t ft8;  // f28
    flen_t ft9;  // f29
    flen_t ft10; // f30
    flen_t ft11; // f31
#endif
} bm_register_file_t;

typedef void (*bm_intr_handler_t)(bm_register_file_t *stacked_regs);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Set function to handle given interrupt cause
 *
 * \param source Interrupt source to handle
 * \param func Function to set as the handler
 */
void bm_interrupt_set_handler(bm_interrupt_source_t source, bm_intr_handler_t func);

/**
 * \brief Set function to handle given exception cause
 *
 * \param source Exception source to handle
 * \param func Function to set as the handler
 */
void bm_exception_set_handler(bm_exception_source_t source, bm_intr_handler_t func);

/**
 * \brief Set function to handle given external interrupt source
 *
 * \param ext_irq_id External interrupt source ID
 * \param func Function to set as the handler
 */
void bm_ext_irq_set_handler(unsigned ext_irq_id, bm_intr_handler_t func);

/**
 * \brief Installs the interrupt hander for the interrupt framework
 *
 * \param priv_mode Privilege mode for the handler
 */
void bm_interrupt_install_handlers(bm_priv_mode_t priv_mode);

/**
 * \brief Initialize interrupt handling for given privilege mode and interrupt or exceptions sources
 *
 * \param priv_mode Privilege mode to initialize the interrupt handling for
 */
void bm_interrupt_init(bm_priv_mode_t priv_mode);

/**
 * \brief Initialize external interrupt handler device
 */
void bm_ext_irq_init(void);

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

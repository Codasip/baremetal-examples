/* Copyright 2024-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/clic.h"

#include "baremetal/common.h"
#include "baremetal/platform.h"
#include "baremetal/verbose.h"

#include <stdbool.h>
#include <stdint.h>

/** \brief CLINT Interrupts */
typedef enum {
    // clang-format off
    CLIC_MSIP_INPUT_ID       = 0,
    CLIC_MTIP_INPUT_ID       = 1,
#ifdef CLIC_TARGET_EXT_S
    #error "CLIC_TARGET_EXT_S is currently unsupported"
    // CLIC_SSIP_INPUT_ID    = ? Not implemented on L730 FPGA platform, as it's
    //                           not routed from the ACLINT to CLIC. but it
    //                           seems it's routed to the classic RISC-V core
    //                           interrupt source BM_INTERRUPT_SSIP.
    // CLIC_STIP_INPUT_ID    = X Not part of ACLINT design
#endif
    CLIC_NUM_INTERNAL_INPUTS
    // clang-format on
} bm_clic_intr_t;

#define CLIC_NUM_EXTERNAL_INPUTS (TARGET_CLIC_NUM_INPUTS - CLIC_NUM_INTERNAL_INPUTS)

#define CLICINTATTR_MODE_OFFSET  6
#define CLICINTATTR_MODE_MASK    0x3
#define CLICINTATTR_TRIG_OFFSET  1
#define CLICINTATTR_TRIG_MASK    0x3
#define CLICINTATTR_SHV_OFFSET   0
#define CLICINTATTR_SHV_MASK     0x1

unsigned bm_clic_get_ext_irq_id(unsigned ext_irq_id)
{
    if (ext_irq_id >= CLIC_NUM_EXTERNAL_INPUTS)
    {
        bm_fatal("unsupported external interrupt request %u", ext_irq_id);
    }
    return ext_irq_id + CLIC_NUM_INTERNAL_INPUTS;
}

unsigned bm_clic_get_irq_id_for_source(bm_interrupt_source_t source)
{
    switch (source)
    {
        case BM_INTERRUPT_MSIP:
            return CLIC_MSIP_INPUT_ID;
        case BM_INTERRUPT_MTIP:
            return CLIC_MTIP_INPUT_ID;
#ifdef CLIC_TARGET_EXT_S
    #error "CLIC_TARGET_EXT_S is currently unsupported"
        case BM_INTERRUPT_SSIP:
            return CLIC_SSIP_INPUT_ID;
        case BM_INTERRUPT_STIP:
            return CLIC_STIP_INPUT_ID;
#endif
        default:
            // The sole purpose of this function is the translation of RISC-V
            // core interrupt sources (BM_INTERRUPT_xx) to the CLIC interrupt
            // IDs. Usually, a system with a CLIC no longer has a designated
            // external core interrupt (BM_INTERRUPT_xEIP), an external
            // interrupt controller (e.g. a PLIC) would just be handled like
            // any other external peripheral that generates interrupts. All
            // these interrupts must be translated via bm_clic_get_ext_irq_id().
            // We cannot do this translation here, because it would make this
            // function return ambiguous IDs.
            bm_fatal("unsupported interrupt source %u", source);
    }
}

void bm_clic_init(bm_clic_t *clic)
{
    clic->regs->CLICCFG = TARGET_CLIC_INTCTLBITS;

    for (unsigned i = 0; i < TARGET_CLIC_NUM_INPUTS; ++i)
    {
        clic->regs->INPUTS[i].CLICINTIP   = 0;
        clic->regs->INPUTS[i].CLICINTIE   = 0;
        clic->regs->INPUTS[i].CLICINTATTR = 0;
        clic->regs->INPUTS[i].CLICINTCTL  = 1; /* When CLICINTCTLBITS == 8 and xnlbits == 8,
                                                  if level == 0 then this is not an active interrupt. */
        /* When xnlbits < 8, then all the levels are greater than zero since the LSBs are assumed to be 1. */
    }
}

void bm_clic_set_enable(bm_clic_t *clic, unsigned clic_irq_id, bool en)
{
    bm_fatal_check_index(clic_irq_id, clic->regs->INPUTS);
    clic->regs->INPUTS[clic_irq_id].CLICINTIE = en;
}

bool bm_clic_get_enable(bm_clic_t *clic, unsigned clic_irq_id)
{
    bm_fatal_check_index(clic_irq_id, clic->regs->INPUTS);
    return clic->regs->INPUTS[clic_irq_id].CLICINTIE;
}

void bm_clic_set_level(bm_clic_t *clic, unsigned clic_irq_id, uint8_t level)
{
    bm_fatal_check_index(clic_irq_id, clic->regs->INPUTS);
    clic->regs->INPUTS[clic_irq_id].CLICINTCTL = level;
}

uint8_t bm_clic_get_level(bm_clic_t *clic, unsigned clic_irq_id)
{
    bm_fatal_check_index(clic_irq_id, clic->regs->INPUTS);
    return clic->regs->INPUTS[clic_irq_id].CLICINTCTL;
}

void bm_clic_set_vectored(bm_clic_t *clic, unsigned clic_irq_id, bool shv)
{
    bm_fatal_check_index(clic_irq_id, clic->regs->INPUTS);
    if (shv)
    {
        clic->regs->INPUTS[clic_irq_id].CLICINTATTR |= (CLICINTATTR_SHV_MASK << CLICINTATTR_SHV_OFFSET);
    }
    else
    {
        clic->regs->INPUTS[clic_irq_id].CLICINTATTR &= ~(CLICINTATTR_SHV_MASK
                                                         << CLICINTATTR_SHV_OFFSET);
    }
}

bool bm_clic_get_pending(bm_clic_t *clic, unsigned clic_irq_id)
{
    bm_fatal_check_index(clic_irq_id, clic->regs->INPUTS);
    return clic->regs->INPUTS[clic_irq_id].CLICINTIP;
}

void bm_clic_clear_pending(bm_clic_t *clic, unsigned clic_irq_id)
{
    clic->regs->INPUTS[clic_irq_id].CLICINTIP = 0;
}

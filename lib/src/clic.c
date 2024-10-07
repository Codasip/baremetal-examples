/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/clic.h"

#include "baremetal/common.h"
#include "baremetal/platform.h"
#include "baremetal/verbose.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef TARGET_EXT_S
    #define CLIC_NUM_INTERNAL_INPUTS 4
    #define CLIC_NUM_EXTERNAL_INPUTS (TARGET_CLIC_NUM_INPUTS - CLIC_NUM_INTERNAL_INPUTS)
    #define CLIC_SSIP_INPUT_ID       0
    #define CLIC_STIP_INPUT_ID       1
    #define CLIC_MSIP_INPUT_ID       2
    #define CLIC_MTIP_INPUT_ID       3
#else
    #define CLIC_NUM_INTERNAL_INPUTS 2
    #define CLIC_NUM_EXTERNAL_INPUTS (TARGET_CLIC_NUM_INPUTS - CLIC_NUM_INTERNAL_INPUTS)
    #define CLIC_MSIP_INPUT_ID       0
    #define CLIC_MTIP_INPUT_ID       1
#endif

#define CLICINTATTR_MODE_OFFSET 6
#define CLICINTATTR_MODE_MASK   0x3
#define CLICINTATTR_TRIG_OFFSET 1
#define CLICINTATTR_TRIG_MASK   0x3
#define CLICINTATTR_SHV_OFFSET  0
#define CLICINTATTR_SHV_MASK    0x1

struct bm_clic_regs {
    volatile uint32_t CLICCFG;
    uint8_t           _reserved1[0xffc];
    /** \brief Structure representing CLIC input configuration registers */
    struct bm_clic_input {
        volatile uint8_t CLICINTIP;
        volatile uint8_t CLICINTIE;
        volatile uint8_t CLICINTATTR;
        volatile uint8_t CLICINTCTL;
    } INPUTS[TARGET_CLIC_NUM_INPUTS];
};

unsigned bm_clic_get_ext_irq_id(unsigned ext_irq_id)
{
    return ext_irq_id + CLIC_NUM_INTERNAL_INPUTS;
}

unsigned bm_clic_get_irq_id(bm_interrupt_source_t irq)
{
    switch (irq)
    {
#ifdef TARGET_EXT_S
        case BM_INTERRUPT_SSIP:
            return CLIC_SSIP_INPUT_ID;
        case BM_INTERRUPT_STIP:
            return CLIC_STIP_INPUT_ID;
#endif
        case BM_INTERRUPT_MSIP:
            return CLIC_MSIP_INPUT_ID;
        case BM_INTERRUPT_MTIP:
            return CLIC_MTIP_INPUT_ID;
        default:
            return bm_clic_get_ext_irq_id(irq);
    }
}

void bm_clic_init(bm_clic_t *clic)
{
    clic->regs->CLICCFG = TARGET_CLIC_INTCTLBITS;

    for (unsigned i = 0; i < TARGET_CLIC_NUM_INPUTS; ++i)
    {
        clic->regs->INPUTS[i].CLICINTIE   = 0;
        clic->regs->INPUTS[i].CLICINTATTR = 0;
        clic->regs->INPUTS[i].CLICINTCTL  = 0;
    }
}

void bm_clic_set_enable(bm_clic_t *clic, unsigned clic_irq_id, bool en)
{
    clic->regs->INPUTS[clic_irq_id].CLICINTIE = en;
}

bool bm_clic_get_enable(bm_clic_t *clic, unsigned clic_irq_id)
{
    return clic->regs->INPUTS[clic_irq_id].CLICINTIE;
}

void bm_clic_set_level(bm_clic_t *clic, unsigned clic_irq_id, uint8_t level)
{
    clic->regs->INPUTS[clic_irq_id].CLICINTCTL = level;
}

uint8_t bm_clic_get_level(bm_clic_t *clic, unsigned clic_irq_id)
{
    return clic->regs->INPUTS[clic_irq_id].CLICINTCTL;
}

void bm_clic_set_vectored(bm_clic_t *clic, unsigned clic_irq_id, bool shv)
{
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
    return clic->regs->INPUTS[clic_irq_id].CLICINTIP;
}

/* Copyright 2023-2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/plic.h"

#include "baremetal/platform.h"

#include <stdbool.h>
#include <stdint.h>

void bm_plic_set_priority(bm_plic_t *plic, unsigned ext_irq_id, uint32_t priority)
{
    // Priorities are indexed by interrupt id, not interrupt source numbers
    // (id + 1, as source 0 does not exist)
    plic->regs->PRIO[ext_irq_id] = priority;
}

uint32_t bm_plic_get_priority(bm_plic_t *plic, unsigned ext_irq_id)
{
    return plic->regs->PRIO[ext_irq_id];
}

void bm_plic_set_threshold(bm_plic_t *plic, unsigned context, uint32_t threshold)
{
    plic->regs->CONTEXT[context].THRESHOLD = threshold;
}

uint32_t bm_plic_get_threshold(bm_plic_t *plic, unsigned context)
{
    return plic->regs->CONTEXT[context].THRESHOLD;
}

void bm_plic_set_enable(bm_plic_t *plic, unsigned context, unsigned ext_irq_id, bool en)
{
    unsigned source = ext_irq_id + 1;
    unsigned index  = source / 32;
    uint32_t mask   = 1 << (source % 32);

    uint32_t enables = bm_plic_get_enables(plic, context, index);

    enables = en ? (enables | mask) : (enables & ~mask);

    bm_plic_set_enables(plic, context, index, enables);
}

void bm_plic_set_enables(bm_plic_t *plic, unsigned context, unsigned index, uint32_t source_mask)
{
    plic->regs->ENABLE[context][index] = source_mask;
}

bool bm_plic_get_enable(bm_plic_t *plic, unsigned context, unsigned ext_irq_id)
{
    unsigned source = ext_irq_id + 1;
    unsigned index  = source / 32;
    uint32_t mask   = 1 << (source % 32);

    return bm_plic_get_enables(plic, context, index) & mask;
}

uint32_t bm_plic_get_enables(bm_plic_t *plic, unsigned context, unsigned index)
{
    return plic->regs->ENABLE[context][index];
}

int bm_plic_claim(bm_plic_t *plic, unsigned context)
{
    return ((int)plic->regs->CONTEXT[context].CLAIM) - 1;
}

void bm_plic_complete(bm_plic_t *plic, unsigned context, unsigned ext_irq_id)
{
    plic->regs->CONTEXT[context].COMPLETE = ext_irq_id + 1;
}

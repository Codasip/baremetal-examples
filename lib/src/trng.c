/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/trng.h"

#include <stdint.h>

struct bm_trng_regs {
    volatile uint32_t RAW;
    volatile uint32_t RAWN;
    volatile uint32_t RND;
    volatile uint32_t RNDN;
    volatile uint32_t CONFIG;
    volatile uint32_t STATUS;
};

uint32_t bm_trng_get_raw(bm_trng_t *trng)
{
    // Wait for available sample
    while (!trng->regs->RAWN)
        ;
    return trng->regs->RAW;
}

uint32_t bm_trng_get_rnd(bm_trng_t *trng)
{
    // Wait for available sample
    while (!trng->regs->RNDN)
        ;
    return trng->regs->RND;
}

uint32_t bm_trng_get_status(bm_trng_t *trng)
{
    return trng->regs->STATUS;
}

void bm_trng_configure(bm_trng_t *trng, const bm_trng_config_t *config)
{
    uint32_t config_word = 0;

    config_word |= ((uint32_t)config->counter_sample_tero) << 0;
    config_word |= ((uint32_t)config->counter_clear_tero) << 8;
    config_word |= ((uint32_t)config->counter_wrap) << 16;

    if (!config->unbiasing)
    {
        config_word |= (1U << 30);
    }

    if (!config->checking)
    {
        config_word |= (1U << 31);
    }

    trng->regs->CONFIG = config_word;
}

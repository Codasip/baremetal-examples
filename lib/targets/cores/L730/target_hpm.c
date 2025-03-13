/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/counter.h"
#include "baremetal/csr.h"
#include "baremetal/hpm.h"
#include "baremetal/verbose.h"

#include <stdint.h>

#define MAX_SUPPORTED_COUNTERS 4

/**
 * \brief Table with bm_counter_id of supported HPM counters
 */
const bm_counter_id supported_hpm_counter_table[MAX_SUPPORTED_COUNTERS] = {
    BM_COUNTER_HPM3,
    BM_COUNTER_HPM4,
    BM_COUNTER_HPM5,
    BM_COUNTER_HPM6,
};

/**
 * \brief Table with bm_hpm_event_t of supported HPM event CSRs
 */
const bm_csr_id supported_hpm_csr_table[MAX_SUPPORTED_COUNTERS] = {
    BM_CSR_MHPMEVENT3,
    BM_CSR_MHPMEVENT4,
    BM_CSR_MHPMEVENT5,
    BM_CSR_MHPMEVENT6,
};

static int claimed_hpm_counter_table[MAX_SUPPORTED_COUNTERS] = {-1, -1, -1, -1};

/** \brief Helper function for retrieving available or reserved counter ID */
static inline int get_counter_id(bm_hpm_event_t event)
{
    int avail = -1;
    for (int i = 0; i < MAX_SUPPORTED_COUNTERS; ++i)
    {
        if (claimed_hpm_counter_table[i] == (int)event)
        {
            return supported_hpm_counter_table[i];
        }
        if (claimed_hpm_counter_table[i] == -1)
        {
            avail = i;
        }
    }

    if (avail != -1)
    {
        claimed_hpm_counter_table[avail] = event;
        bm_csr_write(supported_hpm_csr_table[avail], (xlen_t)event);
        return supported_hpm_counter_table[avail];
    }

    bm_warn("No available HPM counter");
    return -1;
}

int bm_hpmcounter_pause(bm_hpm_event_t event)
{
    int counter = get_counter_id(event);
    if (counter == -1)
    {
        return -1;
    }
    bm_counter_pause((bm_counter_id)counter);
    return 0;
}

int bm_hpmcounter_resume(bm_hpm_event_t event)
{
    int counter = get_counter_id(event);
    if (counter == -1)
    {
        return -1;
    }
    bm_counter_resume((bm_counter_id)counter);
    return 0;
}

int bm_hpmcounter_start(bm_hpm_event_t event)
{
    int counter = get_counter_id(event);
    if (counter == -1)
    {
        return -1;
    }
    bm_counter_clear((bm_counter_id)counter);
    return 0;
}

int bm_hpmcounter_stop(bm_hpm_event_t event)
{
    for (int i = 0; i < MAX_SUPPORTED_COUNTERS; ++i)
    {
        if (claimed_hpm_counter_table[i] == (int)event)
        {
            bm_csr_write(supported_hpm_csr_table[i], 0);
            claimed_hpm_counter_table[i] = -1;
            return 0;
        }
    }
    bm_warn("HPM counter not started");
    return -1;
}

uint64_t bm_hpmcounter_read(bm_hpm_event_t event)
{
    int counter = get_counter_id(event);
    if (counter == -1)
    {
        return (uint64_t)-1;
    }
    return bm_counter_read((bm_counter_id)counter);
}

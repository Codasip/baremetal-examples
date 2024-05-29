/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/counter.h"
#include "baremetal/hpm.h"
#include "baremetal/verbose.h"

#include <stdint.h>

/**
 * \brief Table with bm_counter_id of counters for supported HPM events,
 * and HPM_EVENT_UNSUPPORTED on the indexes of the unsupported events
 */
const int supported_hpm_events_counter_table[] = {
    BM_COUNTER_HPM3,
    BM_COUNTER_HPM4,
    BM_COUNTER_HPM5,
    BM_COUNTER_HPM6,
    BM_COUNTER_HPM9,
    BM_COUNTER_HPM10,
    BM_COUNTER_HPM12,
    BM_COUNTER_HPM13,
    BM_COUNTER_HPM14,
    BM_COUNTER_HPM15,
};

/** \brief Helper function for retrieving counter ID for corresponding HPM events */
static inline int get_counter_id(bm_hpm_event_t event)
{
    if (event >= BM_HPM_EVENTS_COUNT)
    {
        bm_warn("Unsupported HPM event");
        return -1;
    }

    return supported_hpm_events_counter_table[event];
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
    int counter = get_counter_id(event);
    return (counter == -1) ? -1 : 0;
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

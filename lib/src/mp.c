/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/mp.h"

#include "baremetal/csr.h"
#include "baremetal/mem_barrier.h"

#include <stdbool.h>
#include <stddef.h>

/** \brief Structure representing per-hart sync data */
typedef struct {
    bm_hart_func_ptr_t func;
    bm_hart_func_arg_t arg;
    bool               ready;
} bm_hart_sync_data_t;

// Shared data for synchronization between harts
static volatile bm_hart_sync_data_t bm_hart_sync_data[TARGET_NUM_HARTS];

/**
 * \brief Routine called from the startup assembly to manage non-main harts
 */
void __attribute__((noreturn, used)) bm_park_hart(void)
{
    unsigned hart_id = bm_get_hartid();

    // All harts except the main one loop here when inactive
    while (true)
    {
        // Wait until the hart is assigned a job
        while (!bm_hart_sync_data[hart_id].ready)
            ;

        bm_exec_fence();

        // Fetch the jobs data and execute it
        bm_hart_func_arg_t arg  = bm_hart_sync_data[hart_id].arg;
        bm_hart_func_ptr_t func = bm_hart_sync_data[hart_id].func;

        func(arg);

        // Clear the ready flag to signal the job is finished
        bm_hart_sync_data[hart_id].ready = false;
    }
}

int bm_hart_start(unsigned hart_id, bm_hart_func_ptr_t func, bm_hart_func_arg_t arg)
{
    if (bm_hart_sync_data[hart_id].ready)
    {
        // Hart is already running a job
        return -1;
    }

    // Configure parameters for the other hart
    bm_hart_sync_data[hart_id].arg  = arg;
    bm_hart_sync_data[hart_id].func = func;

    bm_exec_fence();

    // Instruct the other hart to start the execution
    bm_hart_sync_data[hart_id].ready = true;

    return 0;
}

bool bm_hart_running(unsigned hart_id)
{
    return bm_hart_sync_data[hart_id].ready;
}

void bm_hart_join(unsigned hart_id)
{
    while (bm_hart_running(hart_id))
        ;
}

void bm_hart_execute_all(bm_hart_func_ptr_t func)
{
    for (unsigned i = 1; i < TARGET_NUM_HARTS; ++i)
    {
        // start job on every other hart
        bm_hart_start(i, func, NULL);
    }

    // run the same job on the main hart
    func(NULL);

    for (unsigned i = 1; i < TARGET_NUM_HARTS; ++i)
    {
        // wait until all jobs finish
        bm_hart_join(i);
    }
}

unsigned bm_get_hartid(void)
{
    return (unsigned)bm_csr_read(BM_CSR_MHARTID);
}

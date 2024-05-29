/**
 * Codasip s.r.o.
 *
 * CONFIDENTIAL
 *
 * Copyright 2023 Codasip s.r.o.
 *
 * All Rights Reserved.
 * This file is part of a Codasip product. No part of this file may be use, copied,
 * modified, or distributed except in accordance with the terms contained in the
 * Codasip license agreement under which you obtained this file.
 */

#include "memory.h"

#include "config.h"

#include <baremetal/mem_barrier.h>

#define PRAGMA(x) _Pragma(#x)

#ifdef __clang__
    #define UNROLL(x) PRAGMA(clang loop unroll_count(x))
#elif __GNUC__
    #define UNROLL(x) PRAGMA(GCC unroll x)
#endif

void read_single_address(volatile width_t *address)
{
    UNROLL(UNROLL_LEN)
    for (unsigned i = 0; i < NUM_ITERATIONS; i++)
    {
        (void)(*address);
    }
}

void read_single_address_fenced(volatile width_t *address)
{
    UNROLL(UNROLL_LEN)
    for (unsigned i = 0; i < NUM_ITERATIONS; i++)
    {
        (void)(*address);
        bm_exec_fence();
    }
}

void read_consecutive_addresses(volatile width_t *address)
{
    UNROLL(UNROLL_LEN)
    for (unsigned i = 0; i < NUM_ITERATIONS; i++)
    {
        (void)(*address);
        address++;
    }
}

void read_consecutive_addresses_fenced(volatile width_t *address)
{
    UNROLL(UNROLL_LEN)
    for (unsigned i = 0; i < NUM_ITERATIONS; i++)
    {
        (void)(*address);
        bm_exec_fence();
        address++;
    }
}

void write_single_address(volatile width_t *address)
{
    UNROLL(UNROLL_LEN)
    for (unsigned i = 0; i < NUM_ITERATIONS; i++)
    {
        (*address) = 0;
    }
}

void write_single_address_fenced(volatile width_t *address)
{
    UNROLL(UNROLL_LEN)
    for (unsigned i = 0; i < NUM_ITERATIONS; i++)
    {
        (*address) = 0;
        bm_exec_fence();
    }
}

void write_consecutive_addresses(volatile width_t *address)
{
    UNROLL(UNROLL_LEN)
    for (unsigned i = 0; i < NUM_ITERATIONS; i++)
    {
        (*address) = 0;
        address++;
    }
}

void write_consecutive_addresses_fenced(volatile width_t *address)
{
    UNROLL(UNROLL_LEN)
    for (unsigned i = 0; i < NUM_ITERATIONS; i++)
    {
        (*address) = 0;
        bm_exec_fence();
        address++;
    }
}

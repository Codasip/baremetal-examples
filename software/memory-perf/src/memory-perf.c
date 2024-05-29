/**
 * Codasip s.r.o.
 *
 * CONFIDENTIAL
 *
 * Copyright 2024 Codasip s.r.o.
 *
 * All Rights Reserved.
 * This file is part of a Codasip product. No part of this file may be use, copied,
 * modified, or distributed except in accordance with the terms contained in the
 * Codasip license agreement under which you obtained this file.
 */

#include "config.h"
#include "memory.h"

#include <baremetal/common.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Type for an execution function that tests memory
 */
typedef void (*memtest_t)(volatile width_t *addr);

/**
 * Description of memory throughput measurement
 */
struct measurement {
    const char *name;    /**< Measurement name (to be printed) */
    memtest_t   func;    /**< Function to execute */
    xlen_t      address; /**< Base address for the function to work on */
};

/**
 * Array of measurements to be performed
 */
const struct measurement MEASUREMENTS[] = {
    {"SINGLE READ",           read_single_address,                ADDR_START},
    {"SINGLE READ (F)",       read_single_address_fenced,         ADDR_START},
    {"CONSECUTIVE READ",      read_consecutive_addresses,         ADDR_START},
    {"CONSECUTIVE READ (F)",  read_consecutive_addresses_fenced,  ADDR_START},
    {"SINGLE WRITE",          write_single_address,               ADDR_START},
    {"SINGLE WRITE (F)",      write_single_address_fenced,        ADDR_START},
    {"CONSECUTIVE WRITE",     write_consecutive_addresses,        ADDR_START},
    {"CONSECUTIVE WRITE (F)", write_consecutive_addresses_fenced, ADDR_START},
    {NULL,                    NULL,                               0         }
};

/**
 * \brief Return the number of CPU cycles since startup
 */
static xlen_t get_cycles(void)
{
    xlen_t value;
    __asm__ volatile("csrr %0, mcycle" : "=r"(value));
    return value;
}

/**
 * \brief Run memory throughput measurement and report results
 *
 * \param[in] m  Measurement description
 */
static void run_measurement(const struct measurement *m)
{
    xlen_t before, after, elapsed;
    double per_iteration;
    before = get_cycles();
    m->func((volatile void *)m->address);
    after = get_cycles();

    elapsed       = after - before;
    per_iteration = (double)elapsed / NUM_ITERATIONS;

    uint64_t bytes = sizeof(width_t) * NUM_ITERATIONS;
    double   mb    = bytes / 1024.0 / 1024.0;
    double   mbps  = mb * TARGET_CLK_FREQ / elapsed;

    printf("Measuring %-26s: cycles %9lu, per access %6.3lf, %6.3lf MB/s\n",
           m->name,
           (unsigned long)elapsed,
           per_iteration,
           mbps);
}

/**
 * \brief Print program configuration (see config.h)
 */
static void print_config(void)
{
    uint64_t bytes = sizeof(width_t) * NUM_ITERATIONS;
    double   mb    = bytes / 1024.0 / 1024.0;

    printf("CPU frequency:            : %6.3lf MHz\n", TARGET_CLK_FREQ / 1000.0 / 1000.0);
    printf("Configuration:\n");
    printf("  - Access width (bytes)  : %u\n", (unsigned)sizeof(width_t));
    printf("  - Loop unrolling        : %u\n", (unsigned)UNROLL_LEN);
    printf("  - Number of accesses    : %u (%6.3lf MB)\n", (unsigned)NUM_ITERATIONS, mb);
    printf("\n");
}

int main(void)
{
    read_single_address((width_t *)ADDR_START);

    printf("--------------------------------------------------------------------------------\n");
    print_config();

    const struct measurement *m = MEASUREMENTS;

    while (m->name)
    {
        run_measurement(m);
        m++;
    }

    exit(0);
}

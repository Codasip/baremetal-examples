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
typedef void (*memory_access_t)(volatile xlen_t *addr);

// Get program data start and end addresses for overlap checks
extern int _start;
extern int _end;
xlen_t     program_data_start = (xlen_t)(uintptr_t)(&_start);
xlen_t     program_data_end   = (xlen_t)(uintptr_t)(&_end);

/**
 * \brief Find a single safe memory block within a specified memory range.
 *
 * \param range_start The start address of the memory region to search.
 * \param range_end   The end address of the memory region to search (exclusive).
 * \return A safe address within the range where the memory block fits, or
 *         0 if no suitable space is available.
 */
static xlen_t find_safe_address_in_range(xlen_t range_start, xlen_t range_end)
{
    xlen_t block_size = NUM_ITERATIONS * sizeof(xlen_t);
    xlen_t addr       = range_start;

    if (addr >= program_data_start && addr < program_data_end)
    {
        addr = program_data_end; // move past program/data
    }

    // printf("Addr is -      0x%X\n", addr);
    // printf("Check add is - 0x%X\n", addr + block_size);
    // printf("End of SRAM -  0x%X\n", (xlen_t)SRAM_ADDR_END);

    if (addr + block_size <= range_end)
    {
        return addr; // safe block found
    }
    else
    {
        return 0; // no safe space
    }
}

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
 * \brief Run measurement for one function
 * 
 * \param name Memory access type name
 * \param func Memory access function
 * \param address Memory address
 */
static void run_measurement(const char *name, memory_access_t mem_access, xlen_t address)
{
    xlen_t before, after, elapsed;
    double per_iteration;

    before = get_cycles();
    mem_access((volatile void *)address);
    after = get_cycles();

    elapsed       = after - before;
    per_iteration = (double)elapsed / NUM_ITERATIONS;

    uint64_t bytes = sizeof(xlen_t) * NUM_ITERATIONS;
    double   mb    = bytes / 1024.0 / 1024.0;
    double   mbps  = mb * TARGET_CLK_FREQ / elapsed;

    printf("Measuring %-26s @ 0x%08lx: cycles %9lu, per access %6.3lf, %6.3lf MB/s\n",
           name,
           (unsigned long)address,
           (unsigned long)elapsed,
           per_iteration,
           mbps);
}

/**
 * \brief Run all measurements for all addresses
 */
static void run_all_measurements(const xlen_t *addr)
{
    run_measurement("SINGLE READ", read_single_address, *addr);
    run_measurement("SINGLE READ (F)", read_single_address_fenced, *addr);
    run_measurement("CONSECUTIVE READ", read_consecutive_addresses, *addr);
    run_measurement("CONSECUTIVE READ (F)", read_consecutive_addresses_fenced, *addr);
    run_measurement("SINGLE WRITE", write_single_address, *addr);
    run_measurement("SINGLE WRITE (F)", write_single_address_fenced, *addr);
    run_measurement("CONSECUTIVE WRITE", write_consecutive_addresses, *addr);
    run_measurement("CONSECUTIVE WRITE (F)", write_consecutive_addresses_fenced, *addr);
}

/**
 * \brief Print program configuration (see config.h)
 */
static void print_config(void)
{
    uint64_t bytes = sizeof(xlen_t) * NUM_ITERATIONS;
    double   mb    = bytes / 1024.0 / 1024.0;

    printf("CPU frequency:            : %6.3lf MHz\n", TARGET_CLK_FREQ / 1000.0 / 1000.0);
    printf("Configuration:\n");
    printf("  - Access width (bytes)  : %u\n", (unsigned)sizeof(xlen_t));
    printf("  - Loop unrolling        : %u\n", (unsigned)UNROLL_LEN);
    printf("  - Number of accesses    : %u (%6.3lf MB)\n", (unsigned)NUM_ITERATIONS, mb);
    printf("\n");
}

int main(void)
{
    puts("Welcome to the Memory performance demo!\n");

    print_config();

    xlen_t safe_addr;

    puts("\n================================\n");

    printf("Measuring DDR:\n\n");

    // Run measurements in DDR address range
    safe_addr = find_safe_address_in_range(DDR_ADDR_START, DDR_ADDR_END);
    if (safe_addr)
    {
        printf("Safe address found at " BM_FMT_XLEN "\n", safe_addr);
        run_all_measurements(&safe_addr);
    }
    else
    {
        printf("Not enough safe memory available. Please reduce the size of test data. Skipping "
               "DDR...\n");
    }

    puts("\n================================\n");

    printf("Measuring SRAM:\n\n");

    // Run measurements in SRAM address range
    safe_addr = find_safe_address_in_range(SRAM_ADDR_START, SRAM_ADDR_END);
    if (safe_addr)
    {
        printf("Safe address found at " BM_FMT_XLEN "\n", safe_addr);
        run_all_measurements(&safe_addr);
    }
    else
    {
        printf("Not enough safe memory available. Please reduce the size of test data. Skipping "
               "SRAM...\n");
    }

    puts("\n================================\n");

    exit(0);
}

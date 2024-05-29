/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/interrupt.h>
#include <baremetal/pmp.h>
#include <baremetal/priv.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_RANGE 0x1000

extern int _end;
xlen_t     program_data_end = (xlen_t)(uintptr_t)(&_end);

static volatile bool got_error;

/**
 * \brief Handler for Load Access Failed exception
 */
void load_access_failed_handler(void)
{
    got_error = true;

    puts("Read access failed!");

    // Move past the offending instruction to continue
    bm_csr_write(BM_CSR_MEPC, bm_csr_read(BM_CSR_MEPC) + 0x4);
}

/**
 * \brief Handler for Store Access Failed exception
 */
void store_access_failed_handler(void)
{
    got_error = true;

    puts("Write access failed!");

    // Move past the offending instruction to continue
    bm_csr_write(BM_CSR_MEPC, bm_csr_read(BM_CSR_MEPC) + 0x4);
}

/**
 * \brief Helper function trying load and store on given address
 *
 * \param address Memory address to attempt access at
 */
static inline void test_read_write(xlen_t address)
{
    got_error = false;

    __asm__ volatile("lw x0, 0x0(%0)\n"
                     "sw x0, 0x0(%0)" ::"r"(address));

    if (!got_error)
    {
        puts("Ok.");
    }
}

/**
 * \brief Function testing memory accesses on all configured ranges
 */
static inline void test_memory_access()
{
    puts("Range with read and write access alowed globally:");
    test_read_write(program_data_end + 4);
    puts("Range with write access restricted globally:");
    test_read_write(program_data_end + 4 + 1 * TEST_RANGE);
    puts("Range with read and write access restricted globally:");
    test_read_write(program_data_end + 4 + 2 * TEST_RANGE);
    puts("Range with read and write access alowed in user mode:");
    test_read_write(program_data_end + 4 + 3 * TEST_RANGE);
    puts("Range with write access restricted in user mode:");
    test_read_write(program_data_end + 4 + 4 * TEST_RANGE);
    puts("Range with read and write access restricted in user mode:");
    test_read_write(program_data_end + 4 + 5 * TEST_RANGE);
    puts("Test finished.\n");
}

uint8_t u_stack[0x4000] __attribute__((aligned(16)));

/**
 * \brief Function executed in user mode
 */
void __attribute__((noinline)) entry_user(void)
{
    puts("Accessing in user mode:");
    test_memory_access();

    puts("Bye.");
    exit(EXIT_SUCCESS);
}

int main(void)
{
    puts("Welcome to the PMP demo!\n");

    // Setup interrupt handling
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);

    bm_exception_set_handler(BM_EXCEPTION_LAF, load_access_failed_handler);
    bm_exception_set_handler(BM_EXCEPTION_SAF, store_access_failed_handler);

    puts("Accessing in machine mode before PMP restriction:");
    test_memory_access();

    // Setup PMP to allow execution in U/S modes.
    bm_pmp_set(0,
               program_data_end,
               BM_PMP_MODE_TOR,
               BM_PMP_CONFIG_READ | BM_PMP_MODE_WRITE | BM_PMP_MODE_EXECUTE);

    // Create additional ranges with restricted access rights, to check the PMP functionality
    bm_pmp_set(1,
               program_data_end + TEST_RANGE,
               BM_PMP_MODE_TOR,
               BM_PMP_MODE_WRITE | BM_PMP_CONFIG_READ | BM_PMP_MODE_LOCK);
    bm_pmp_set(2, program_data_end + 2 * TEST_RANGE, BM_PMP_MODE_TOR, BM_PMP_CONFIG_READ | BM_PMP_MODE_LOCK);
    bm_pmp_set(3, program_data_end + 3 * TEST_RANGE, BM_PMP_MODE_TOR, BM_PMP_MODE_LOCK);
    bm_pmp_set(4, program_data_end + 4 * TEST_RANGE, BM_PMP_MODE_TOR, BM_PMP_MODE_WRITE | BM_PMP_CONFIG_READ);
    bm_pmp_set(5, program_data_end + 5 * TEST_RANGE, BM_PMP_MODE_TOR, BM_PMP_CONFIG_READ);
    bm_pmp_set(6, program_data_end + 6 * TEST_RANGE, BM_PMP_MODE_TOR, 0);

    // Setup PMP to allow access to the remaining address range, namely peripherals
    bm_pmp_set(7, 0xffffffff, BM_PMP_MODE_TOR, BM_PMP_CONFIG_READ | BM_PMP_MODE_WRITE | BM_PMP_MODE_EXECUTE);

    puts("Accessing in machine mode:");
    test_memory_access();

    // Enter user mode
    xlen_t ustack = (xlen_t)(u_stack + sizeof(u_stack));
    bm_priv_enter_mode(BM_PRIV_MODE_USER, (xlen_t)entry_user, ustack);
}

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <baremetal/csr.h>
#include <baremetal/interrupt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef TARGET_HAS_CACHE
    #include <target_cache.h>
#endif

#ifdef TARGET_HAS_TCM
    #include <target_tcm.h>
#endif

#define TEST_START      0x80000000
#define TEST_END        0xC0000000
#define TEST_FAST       1
#define DEBUG           0
#define ERROR_THRESHOLD 128
#define FAST_THRESHOLD  0x100000

extern int _start;
extern int _end;
xlen_t     program_data_start = (xlen_t)(uintptr_t)(&_start);
xlen_t     program_data_end   = (xlen_t)(uintptr_t)(&_end);

// clang-format off
#define MEM_READ(inst, addr, val)        \
    __asm__ volatile(".option push\n"    \
                     ".option norvc\n"   \
                     inst " %0, 0(%1)\n" \
                     ".option pop\n"     \
                     : "=r"(val)         \
                     : "r"(addr));

#define MEM_WRITE(inst, addr, val)       \
    __asm__ volatile(".option push\n"    \
                     ".option norvc\n"   \
                     inst " %0, 0(%1)\n" \
                     ".option pop\n"     \
                     :: "r"(val), "r"(addr));
// clang-format on

static unsigned error_count = 0;
static bool     error_flag  = false;

void log_error(void)
{
    error_count++;
    error_flag = true;
    if (error_count > ERROR_THRESHOLD)
    {
        puts("Too many errors, stopping.");
        exit(1);
    }
}

void mem_error_handler(void)
{
    xlen_t mcause = bm_csr_read(BM_CSR_MCAUSE);
    xlen_t mtval  = bm_csr_read(BM_CSR_MTVAL);

    printf("Failed to %s at " BM_FMT_XLEN "\n", mcause == BM_EXCEPTION_LAF ? "read" : "write", mtval);

    log_error();

    // Move past offending instrcution to continue
    bm_csr_write(BM_CSR_MEPC, bm_csr_read(BM_CSR_MEPC) + 0x4);
}

void do_write(xlen_t address, unsigned size, xlen_t value)
{
    switch (size)
    {
        case 8:
            MEM_WRITE("sb", address, value);
            break;
        case 16:
            MEM_WRITE("sh", address, value);
            break;
        case 32:
            MEM_WRITE("sw", address, value);
            break;
#if __riscv_xlen == 64
        case 64:
            MEM_WRITE("sd", address, value);
            break;
#endif
        default:
            printf("Unsupported access width (%u)!", size);
            exit(1);
    }
}

xlen_t do_read(xlen_t address, unsigned size)
{
    xlen_t value = 0;

    switch (size)
    {
        case 8:
            MEM_READ("lb", address, value);
            break;
        case 16:
            MEM_READ("lh", address, value);
            break;
        case 32:
            MEM_READ("lw", address, value);
            break;
#if __riscv_xlen == 64
        case 64:
            MEM_READ("ld", address, value);
            break;
#endif
        default:
            printf("Unsupported access width (%u)!", size);
            exit(1);
    }

    return value;
}

void read_test(xlen_t offset, unsigned size, xlen_t test_value)
{
    error_flag = false;

    xlen_t mask     = ((xlen_t)-1) >> (__riscv_xlen - size);
    xlen_t read_val = do_read(offset, size) & mask;
    xlen_t cur_val  = test_value & mask;
#if DEBUG
    printf("DEBUG: %3u-bit read at " BM_FMT_XLEN ":  " BM_FMT_XLEN "\n", size, offset, read_val);
#endif
    if (!error_flag && read_val != cur_val)
    {
        printf("Incorrect value read at " BM_FMT_XLEN "!\n", offset);
        printf("Expected " BM_FMT_XLEN ", read " BM_FMT_XLEN ".\n", cur_val, read_val);
        log_error();
    }
}

void write_test(xlen_t offset, unsigned size, xlen_t test_value)
{
#if DEBUG
    xlen_t mask = ((xlen_t)-1) >> (__riscv_xlen - size);
    printf("DEBUG: %3u-bit write at " BM_FMT_XLEN ": " BM_FMT_XLEN "\n", size, offset, test_value & mask);
#endif
    do_write(offset, size, test_value);
}

unsigned constant_size(void)
{
    return 32;
}

/**
 * Iterate over all available access widths
 * 8 -> 16 -> .. -> XLEN
 */
unsigned iterative_size(void)
{
    static unsigned size = __riscv_xlen;

    size = (size == __riscv_xlen) ? 8 : size * 2;

    return size;
}

/**
 * Iterate over all available access widths, repeating each width multiple times
 * to ensure all pairs are tested when combined with iterative_size.
 * For RV32 this generates:
 * 8 -> 8 -> 8 -> 16 -> 16 -> 16 -> 32 -> 32 -> 32
 * while iterative_size generates:
 * 8 -> 16 -> 32 -> 8 -> 16 -> 32 -> 8 -> 16 -> 32
 */
unsigned iterative_size_squared(void)
{
    static unsigned size        = __riscv_xlen;
    const unsigned  repetitions = (__riscv_xlen == 64) ? 4 : 3;
    static unsigned repeat      = repetitions;

    repeat = (repeat == 1) ? repetitions : repeat - 1;
    if (repeat != repetitions)
    {
        return size;
    }

    size = (size == __riscv_xlen) ? 8 : size * 2;

    return size;
}

/**
 * \brief Iterate over given memory range, generating pseudorandom values and accessing memory using given functions.
 *
 * \param start Start of the memory to test
 * \param end End of the memory to test
 * \param mem_access Memory access function to test
 * \param size_gen Function to generate memory access widths
 */
void iterate_range(xlen_t start,
                   xlen_t end,
                   void (*mem_access)(xlen_t offset, unsigned size, xlen_t cur_val),
                   unsigned (*size_gen)(void))
{
    srand(0);

    for (xlen_t address = start; address < end; address += __riscv_xlen / 8)
    {
        unsigned size       = size_gen();
        xlen_t   test_value = rand();
#if __riscv_xlen == 64
        test_value <<= 32;
        test_value |= rand();
#endif

        for (xlen_t offset = address; offset < address + __riscv_xlen / 8; offset += size / 8)
        {
            mem_access(offset, size, test_value);

            test_value >>= size;
        }
    }
}

/**
 * \brief Test given memory range, first writes test values, then reads out and checks the values
 *
 * \param start Start of the memory to test
 * \param end End of the memory to test
 * \param fast Toggle a fast mode testing
 */
void test(xlen_t start, xlen_t end, bool fast)
{
    bool test_whole_range        = !fast || (end - start < 2 * FAST_THRESHOLD);
    unsigned (*size_gen_w)(void) = fast ? constant_size : iterative_size;
    unsigned (*size_gen_r)(void) = fast ? constant_size : iterative_size_squared;

    if (test_whole_range)
    {
        iterate_range(start, end, write_test, size_gen_w);
    }
    else
    {
        iterate_range(start, start + FAST_THRESHOLD, write_test, size_gen_w);
        iterate_range(end - FAST_THRESHOLD, end, write_test, size_gen_w);
    }

#ifdef TARGET_HAS_CACHE
    // Flush and invalidate caches
    bm_dcache_invalidate_all();
#endif

    if (test_whole_range)
    {
        iterate_range(start, end, read_test, size_gen_r);
    }
    else
    {
        iterate_range(start, start + FAST_THRESHOLD, read_test, size_gen_r);
        iterate_range(end - FAST_THRESHOLD, end, read_test, size_gen_r);
    }
}

int main(void)
{
    printf("Testing memory range from 0x%llx to 0x%llx, mode: %s.\n",
           (unsigned long long)TEST_START,
           (unsigned long long)TEST_END,
           TEST_FAST ? "fast" : "normal");

    if (program_data_start < (xlen_t)TEST_END && (xlen_t)TEST_START < program_data_end)
    {
        printf("WARNING: tested range overlaps program range (" BM_FMT_XLEN " - " BM_FMT_XLEN
               "), program may fail or loop!\n",
               program_data_start,
               program_data_end);
    }

#ifdef TARGET_HAS_TCM
    // enable TCMs if present to allow testing their memory ranges
    bm_tcm_itcm_enable();
    bm_tcm_dtcm_enable();
#endif

    // Initialize exception handling
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);
    bm_exception_set_handler(BM_EXCEPTION_LAF, mem_error_handler);
    bm_exception_set_handler(BM_EXCEPTION_SAF, mem_error_handler);

    test((xlen_t)TEST_START, (xlen_t)TEST_END, TEST_FAST);

    printf("Test %s\n", error_count == 0 ? "passed." : "failed!");

    return error_count;
}

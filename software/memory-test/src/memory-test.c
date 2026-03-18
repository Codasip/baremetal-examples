/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/bm_cheri.h>
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

// =====================================
// Memory regions to be tested
// =====================================

#define DDR_ADDR_START  0x80000000
#define DDR_ADDR_END    0xC0000000

#define SRAM_ADDR_START 0x20000000
#define SRAM_ADDR_END   0x20080000

// =====================================
// Test configuration
// =====================================

#define MEM_FAST_ACCESS_WIDTH 32

#define TEST_SAFE             1
#define TEST_FAST             1

#define DEBUG                 0

#define ERROR_THRESHOLD       128
#define FAST_THRESHOLD        0x100000

#define INSTR_SIZE            0x04

// =====================================
// Memory access macros
// =====================================

// clang-format off
#ifdef __CHERI_PURE_CAPABILITY__
#define MEM_READ(inst, addr, val)        \
    __asm__ volatile(".option push\n"    \
                     ".option norvc\n"   \
                     inst " %0, 0(%1)\n" \
                     ".option pop\n"     \
                     : "=r"(val)         \
                     : "C"(addr));

#define MEM_WRITE(inst, addr, val)       \
    __asm__ volatile(".option push\n"    \
                     ".option norvc\n"   \
                     inst " %0, 0(%1)\n" \
                     ".option pop\n"     \
                     :: "r"(val), "C"(addr));

#else
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
#endif
// clang-format on

// Typedef for memory size generator and mem access functions
typedef unsigned (*mem_size_gen_t)(void);
typedef void (*mem_access_t)(xlen_t offset, unsigned size, xlen_t cur_val);

// Get program data start and end addresses for overlap checks
extern int _start;
extern int _end;
extern int __heap_start;
extern int __heap_end;
extern int __stack_start;
extern int __stack_size;
xlen_t     program_data_start = (xlen_t)(uintptr_t)(&_start);
xlen_t     program_data_end   = (xlen_t)(uintptr_t)(&_end);
xlen_t     heap_start         = (xlen_t)(uintptr_t)(&__heap_start);
xlen_t     heap_end           = (xlen_t)(uintptr_t)(&__heap_end);
xlen_t     stack_start        = (xlen_t)(uintptr_t)(&__stack_start);
xlen_t     stack_size         = (xlen_t)&__stack_size;

// Error count mechanism global variables
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

void mem_error_handler(bm_register_file_t *stacked_regs)
{
    (void)stacked_regs;

    xlen_t mcause = 0;
    xlen_t mtval  = 0;

    BM_CSR_READ(BM_CSR_MCAUSE, mcause);
    BM_CSR_READ(BM_CSR_MTVAL, mtval);

    printf("Failed to %s at " BM_FMT_XLEN "\n", mcause == BM_EXCEPTION_LAF ? "read" : "write", mtval);

    log_error();

    // Move past offending instruction to continue
#ifdef __CHERI_PURE_CAPABILITY__
    const uint8_t *csr_val = 0;

    BM_CSR_READ_CAP(mepcc, csr_val);
    BM_CSR_WRITE_CAP(mepcc, csr_val + INSTR_SIZE);

#else
    xlen_t csr_val = 0;
    BM_CSR_READ(BM_CSR_MEPC, csr_val);
    BM_CSR_WRITE(BM_CSR_MEPC, csr_val + INSTR_SIZE);
#endif
}

void do_write(xlen_t address_in, unsigned size, xlen_t value)
{
#ifdef __CHERI_PURE_CAPABILITY__
    const void *address = addr_to_data_ptr(address_in, 8);
#else
    xlen_t address = address_in;
#endif

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
#if RISCV_XLEN == 64
        case 64:
            MEM_WRITE("sd", address, value);
            break;
#endif
        default:
            printf("Unsupported access width (%u)!", size);
            exit(1);
    }
}

xlen_t do_read(xlen_t address_in, unsigned size)
{
#ifdef __CHERI_PURE_CAPABILITY__
    const void *address = addr_to_data_ptr(address_in, 8);
#else
    xlen_t address = address_in;
#endif

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
#if RISCV_XLEN == 64
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

    xlen_t mask     = ((xlen_t)-1) >> (RISCV_XLEN - size);
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
    xlen_t mask = ((xlen_t)-1) >> (RISCV_XLEN - size);
    printf("DEBUG: %3u-bit write at " BM_FMT_XLEN ": " BM_FMT_XLEN "\n", size, offset, test_value & mask);
#endif
    do_write(offset, size, test_value);
}

/**
 * Returns a fixed memory access width for fast memory tests
 */
unsigned mem_constant_size(void)
{
    return MEM_FAST_ACCESS_WIDTH;
}

/**
 * Returns next memory access width in sequence (8->16->32)
 */
unsigned mem_iterative_size(void)
{
    static unsigned size = RISCV_XLEN;

    size = (size == RISCV_XLEN) ? 8 : size * 2;

    return size;
}

/**
 * Returns next memory access width in repeated sequence for pairwise testing
 */
unsigned mem_iterative_size_repeated(void)
{
    static unsigned size        = RISCV_XLEN;
    const unsigned  repetitions = (RISCV_XLEN == 64) ? 4 : 3;
    static unsigned repeat      = repetitions;

    repeat = (repeat == 1) ? repetitions : repeat - 1;
    if (repeat != repetitions)
    {
        return size;
    }

    size = (size == RISCV_XLEN) ? 8 : size * 2;

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
void iterate_range(xlen_t start, xlen_t end, mem_access_t mem_access, mem_size_gen_t size_gen)
{
    srand(0);

    for (xlen_t address = start; address < end; address += RISCV_XLEN / 8)
    {
        unsigned size       = size_gen();
        xlen_t   test_value = rand();

#if RISCV_XLEN == 64
        test_value <<= 32;
        test_value |= rand();
#endif

        for (xlen_t offset = address; offset < address + RISCV_XLEN / 8; offset += size / 8)
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
 * \param fast_mode_enabled Toggle a fast mode testing
 */
void test(xlen_t start, xlen_t end, bool fast_mode_enabled)
{
    bool           test_whole_range = !fast_mode_enabled || (end - start < 2 * FAST_THRESHOLD);
    mem_size_gen_t size_gen_w       = fast_mode_enabled ? mem_constant_size : mem_iterative_size;
    mem_size_gen_t size_gen_r = fast_mode_enabled ? mem_constant_size : mem_iterative_size_repeated;

    printf("Testing memory range from 0x%llx to 0x%llx, mode: %s.\n",
           (unsigned long long)start,
           (unsigned long long)end,
           fast_mode_enabled ? "fast" : "normal");

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

/**
 * \brief Safely test a memory range, optionally skipping program data to avoid corruption. Use this function if tested memory range may contain program data.
 *
 * \param start_addr Start address of the memory range
 * \param end_addr End address of the memory range
 * \param fast_mode_enabled Enable fast mode
 * \param safe_test_enabled Enable safe mode to skips program data
 */
void safe_test(xlen_t start, xlen_t end, bool fast_mode_enabled, bool safe_mode_enabled)
{
    xlen_t stack_end  = stack_start + stack_size;
    xlen_t addr_start = start;
    xlen_t addr_end   = end;
    bool   overlaps   = false;

    if (addr_start >= program_data_start && addr_start < program_data_end)
    {
        overlaps   = true;
        addr_start = program_data_end; // move past program/data
    }

    if (addr_start >= heap_start && addr_start < heap_end)
    {
        overlaps   = true;
        addr_start = heap_end; // move past heap
    }

    if (addr_start >= stack_start && addr_start < stack_end)
    {
        overlaps   = true;
        addr_start = stack_end; // move past stack
    }

    if (addr_end >= program_data_start && addr_end < program_data_end)
    {
        overlaps = true;
        addr_end = program_data_start; // move before program/data
    }

    if (addr_end >= heap_start && addr_end < heap_end)
    {
        overlaps = true;
        addr_end = heap_start; // move before heap
    }

    if (addr_end >= stack_start && addr_end < stack_end)
    {
        overlaps = true;
        addr_end = stack_start; // move before stack
    }

    if (!safe_mode_enabled)
    {
        // Safe mode is disabled: warn if overlap exists
        if (overlaps)
        {
            printf("WARNING: SAFE TEST DISABLED!\n");
            printf("Memory range (" BM_FMT_XLEN " - " BM_FMT_XLEN
                   ") overlaps program/heap/stack data (" BM_FMT_XLEN " - " BM_FMT_XLEN ").\n",
                   start,
                   end,
                   program_data_start,
                   program_data_end);
            printf("This may overwrite code, global variables, or stack!\n");
        }

        test(start, end, fast_mode_enabled);
        return;
    }

    // Safe mode is enabled: skip program/heap/stack data
    if (overlaps)
    {
        printf("SAFE TEST ENABLED: skipping program/heap/stack data range (" BM_FMT_XLEN
               " - " BM_FMT_XLEN ").\n",
               program_data_start,
               stack_end);

        test(addr_start, addr_end, fast_mode_enabled);
    }
    else
    {
        // No overlap: safe test enabled, test full range
        printf("SAFE TEST ENABLED: memory is safe for full test.\n");
        test(start, end, fast_mode_enabled);
    }
}

int main(void)
{
    puts("Welcome to the Memory test demo!\n");

#ifdef TARGET_HAS_TCM
    xlen_t itcm_start, itcm_size;
    xlen_t dtcm_start, dtcm_size;

    // enable TCMs if present to allow testing their memory ranges
    bm_tcm_itcm_enable();
    bm_tcm_dtcm_enable();

    itcm_start = bm_tcm_itcm_get_base_address();
    itcm_size  = bm_tcm_itcm_get_size();

    dtcm_start = bm_tcm_dtcm_get_base_address();
    dtcm_size  = bm_tcm_dtcm_get_size();
#endif

    // Initialize exception handling
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);
    bm_exception_set_handler(BM_EXCEPTION_LAF, mem_error_handler);
    bm_exception_set_handler(BM_EXCEPTION_SAF, mem_error_handler);

#ifdef TARGET_HAS_TCM
    safe_test((xlen_t)itcm_start, (xlen_t)itcm_start + itcm_size, TEST_FAST, TEST_SAFE);
    safe_test((xlen_t)dtcm_start, (xlen_t)dtcm_start + dtcm_size, TEST_FAST, TEST_SAFE);
#endif

    safe_test((xlen_t)DDR_ADDR_START, (xlen_t)DDR_ADDR_END, TEST_FAST, TEST_SAFE);
    safe_test((xlen_t)SRAM_ADDR_START, (xlen_t)SRAM_ADDR_END, TEST_FAST, TEST_SAFE);

    printf("Test %s\n", error_count == 0 ? "passed." : "failed!");

    return error_count;
}

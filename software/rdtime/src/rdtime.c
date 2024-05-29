/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/clint.h>
#include <baremetal/common.h>
#include <baremetal/counter.h>
#include <baremetal/csr.h>
#include <baremetal/interrupt.h>
#include <baremetal/platform.h>
#include <baremetal/pmp.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Pseudoinstruction rdtime is an alias for csrrs rd, time, x0
// encoded in binary as csr[12] | rs[5] | funct[3] | rd[5] | opcode[7]
// where only the destination register rd is variable
#define INST_RDTIME            0xC0102073
#define INST_RDTIMEH           0xC8102073
#define INST_RDTIME_MASK       0xFFFFF07F
#define INST_RDTIME_REG_MASK   0xF80
#define INST_RDTIME_REG_OFFSET 0x7

static bool rdtime_handler_called = false;

void unsuported_instruction_handler(void)
{
    xlen_t   inst_addr   = bm_csr_read(BM_CSR_MEPC);
    xlen_t   instruction = bm_csr_read(BM_CSR_MTVAL);
    unsigned inst_size   = 4; // Has to be updated if compressed instruction is decoded

    if (((instruction & INST_RDTIME_MASK) == INST_RDTIME) ||
        ((instruction & INST_RDTIME_MASK) == INST_RDTIMEH))
    {
        rdtime_handler_called = true;

        // If RDTIME is unsupported, we emulate the instruction using value from CLINT
        bm_clint_t *clint = (bm_clint_t *)target_peripheral_get(BM_PERIPHERAL_CLINT);
        uint64_t    mtime = bm_clint_get_mtime(clint);

        xlen_t reg_val = mtime;
        if ((instruction & INST_RDTIME_MASK) == INST_RDTIMEH)
        {
            reg_val = mtime >> 32;
        }

        // Update register encoded in the instruction's binary
        unsigned reg = (instruction & INST_RDTIME_REG_MASK) >> INST_RDTIME_REG_OFFSET;
        volatile bm_register_file_t *regs = &bm_priv_regs[bm_get_priv_mode()];
        ((xlen_t *)regs)[reg - 1] = reg_val; // x0 register is not saved in the register file
    }
    else
    {
        puts("Invalid instruction!");
        exit(EXIT_FAILURE);
    }

    // Move past the offending instruction to continue
    bm_csr_write(BM_CSR_MEPC, inst_addr + inst_size);
}

static xlen_t get_time(void)
{
    xlen_t result;

    __asm__ volatile("rdtime %0" : "=r"(result) : :);

    return result;
}

uint8_t u_stack[0x4000] __attribute__((aligned(16)));

void __attribute__((noinline)) entry_user(void)
{
    puts("Using rdtime to meassure 100ms delay:");

    xlen_t time_before = get_time();

    bm_delay_ms(100);

    xlen_t time_after = get_time();

    printf("Time before    : %lu\n", (unsigned long)time_before);
    printf("Time after     : %lu\n", (unsigned long)time_after);
    printf("Time elapsed   : %lu\n\n", (unsigned long)(time_after - time_before));

    uint64_t cycles_before = bm_get_cycles();

    const int rep = 1000;
    for (int i = 0; i < rep; ++i)
    {
        get_time();
    }

    unsigned long cycles_elapsed = bm_get_cycles() - cycles_before;

    printf("Using rdtime   : %s\n", rdtime_handler_called ? "Emulation" : "Native");
    printf("Average rdtime : %lu cycles\n\n", cycles_elapsed / rep);

    puts("Bye.");
    exit(EXIT_SUCCESS);
}

int main(void)
{
    puts("Welcome to the rdtime demo!\n");

    // Initialize interrupt and exception handling
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);
    bm_exception_set_handler(BM_EXCEPTION_II, unsuported_instruction_handler);

    // Enable time and cycle counters in user mode
    bm_counter_enable_priv(BM_PRIV_MODE_USER, BM_COUNTER_TIME);
    bm_counter_enable_priv(BM_PRIV_MODE_USER, BM_COUNTER_CYCLE);

#ifdef TARGET_HAS_PMP
    // Setup PMP to allow execution and peripheral access in U/S modes.
    bm_pmp_set(0, 0xffffffff, BM_PMP_MODE_TOR, BM_PMP_CONFIG_READ | BM_PMP_MODE_WRITE | BM_PMP_MODE_EXECUTE);
#endif

    // Enter user mode
    xlen_t stack = (xlen_t)(u_stack + sizeof(u_stack));
    bm_priv_enter_mode(BM_PRIV_MODE_USER, (xlen_t)entry_user, stack);
}

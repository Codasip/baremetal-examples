/* Copyright 2024-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/clint.h>
#include <baremetal/common.h>
#include <baremetal/counter.h>
#include <baremetal/csr.h>
#include <baremetal/interrupt.h>
#include <baremetal/mp.h>
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

void unsuported_instruction_handler(bm_register_file_t *stacked_regs)
{
    const uint8_t *inst_addr   = 0;
    xlen_t         instruction = 0;

    // Get inst_addr and instruction values from CSR registers
#ifdef __CHERI_PURE_CAPABILITY__
    BM_CSR_READ_CAP(mepcc, inst_addr);

#else
    BM_CSR_READ(BM_CSR_MEPC, inst_addr);
#endif

    BM_CSR_READ(BM_CSR_MTVAL, instruction);

    unsigned inst_size = 4; // Has to be updated if compressed instruction is decoded

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

        // If there is no register context, something is seriously broken.
        if (!stacked_regs)
        {
            printf("No register available.");
            exit(EXIT_FAILURE);
        }

        // Update register encoded in the instruction's binary, if possible
        unsigned reg = (instruction & INST_RDTIME_REG_MASK) >> INST_RDTIME_REG_OFFSET;

        switch (reg)
        {
            case 0:
            default:
                /* x0 and some other registers are not saved on the stack */
                printf("Destination register %u not on the stack\n", reg);
                exit(EXIT_FAILURE);
                break;

            case 1:
                stacked_regs->ra = reg_val;
                break;

            case 5:
                stacked_regs->t0 = reg_val;
                break;

            case 6:
                stacked_regs->t1 = reg_val;
                break;

            case 7:
                stacked_regs->t2 = reg_val;
                break;

            case 10:
                stacked_regs->a0 = reg_val;
                break;

            case 11:
                stacked_regs->a1 = reg_val;
                break;

            case 12:
                stacked_regs->a2 = reg_val;
                break;

            case 13:
                stacked_regs->a3 = reg_val;
                break;

            case 14:
                stacked_regs->a4 = reg_val;
                break;

            case 15:
                stacked_regs->a5 = reg_val;
                break;

#ifndef __riscv_32e
            case 16:
                stacked_regs->a6 = reg_val;
                break;

            case 17:
                stacked_regs->a7 = reg_val;
                break;

            case 28:
                stacked_regs->t3 = reg_val;
                break;

            case 29:
                stacked_regs->t4 = reg_val;
                break;

            case 30:
                stacked_regs->t5 = reg_val;
                break;

            case 31:
                stacked_regs->t6 = reg_val;
                break;
#endif
        }
    }
    else
    {
        puts("Invalid instruction!");
        exit(EXIT_FAILURE);
    }

    // Move past the offending instruction to continue
#ifdef __CHERI_PURE_CAPABILITY__
    BM_CSR_WRITE_CAP(mepcc, inst_addr + inst_size);

#else
    BM_CSR_WRITE(BM_CSR_MEPC, inst_addr + inst_size);
#endif
}

static inline xlen_t get_time(void)
{
    xlen_t result;

    __asm__ volatile("rdtime %0" : "=r"(result) : :);

    return result;
}

uint8_t u_stack[0x4000] __attribute__((aligned(16)));

void __attribute__((noinline)) entry_user(void)
{
    puts("Using rdtime to measure 100ms delay:");

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
    uint8_t *stack = u_stack + sizeof(u_stack);
    bm_priv_enter_mode(BM_PRIV_MODE_USER, entry_user, stack);
}

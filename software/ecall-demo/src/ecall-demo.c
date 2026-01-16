/* Copyright 2023-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/interrupt.h>
#include <baremetal/pmp.h>
#include <baremetal/priv.h>
#include <stdio.h>
#include <stdlib.h>

xlen_t call_function(xlen_t arg0, xlen_t arg1, xlen_t arg2)
{
    printf("Parameters: %" BM_FMT_XLEN_T " %" BM_FMT_XLEN_T " %" BM_FMT_XLEN_T "\n", arg0, arg1, arg2);

    return (arg0 + arg1 + arg2);
}

void user_ecall_handler(bm_register_file_t *stacked_regs)
{
    puts("Handling syscall from user mode.");

    if (stacked_regs)
    {
        stacked_regs->a0 = call_function(stacked_regs->a0, stacked_regs->a1, stacked_regs->a2);
    }
    else
    {
        puts("No register context for syscall from user mode, can't do anything.");
    }

    // Move past offending instruction to continue
#ifdef __CHERI_PURE_CAPABILITY__
    const uint8_t *csr_val = 0;

    BM_CSR_READ_CAP(mepcc, csr_val);
    BM_CSR_WRITE_CAP(mepcc, csr_val + 0x04);

#else
    xlen_t csr_val = 0;
    BM_CSR_READ(BM_CSR_MEPC, csr_val);
    BM_CSR_WRITE(BM_CSR_MEPC, csr_val + 0x04);
#endif
}

void machine_ecall_handler(bm_register_file_t *stacked_regs)
{
    puts("Handling syscall from machine mode.");

    if (stacked_regs)
    {
        stacked_regs->a0 = call_function(stacked_regs->a0, stacked_regs->a1, stacked_regs->a2);
    }
    else
    {
        puts("No register context for syscall from machine mode, can't do anything.");
    }

    // Move past offending instruction to continue
#ifdef __CHERI_PURE_CAPABILITY__
    const uint8_t *csr_val = 0;

    BM_CSR_READ_CAP(mepcc, csr_val);
    BM_CSR_WRITE_CAP(mepcc, csr_val + 0x04);

#else
    xlen_t csr_val = 0;
    BM_CSR_READ(BM_CSR_MEPC, csr_val);
    BM_CSR_WRITE(BM_CSR_MEPC, csr_val + 0x04);
#endif
}

uint8_t u_stack[0x4000] __attribute__((aligned(16)));

void __attribute__((noinline)) entry_user(void)
{
    puts("\nHello world from user mode!\n");

    bm_priv_call_args args = {.arg0 = 4, .arg1 = 5, .arg2 = 6};
    puts("Executing call from user mode");
    xlen_t ret = bm_priv_execute_call(args);
    printf("Got %" BM_FMT_XLEN_T ", expected %" BM_FMT_XLEN_T "\n\n",
           ret,
           (args.arg0 + args.arg1 + args.arg2));

    puts("Bye.");
    exit(EXIT_SUCCESS);
}

int main(void)
{
    puts("Welcome to the ecall demo!\n");

    bm_interrupt_init(BM_PRIV_MODE_MACHINE);

    bm_exception_set_handler(BM_EXCEPTION_ECALL_U, user_ecall_handler);
    bm_exception_set_handler(BM_EXCEPTION_ECALL_M, machine_ecall_handler);

#ifdef TARGET_HAS_PMP
    // Setup PMP to allow execution and peripheral access in U/S modes.
    bm_pmp_set(0, 0xffffffff, BM_PMP_MODE_TOR, BM_PMP_CONFIG_READ | BM_PMP_MODE_WRITE | BM_PMP_MODE_EXECUTE);
#endif

    bm_priv_call_args args = {.arg0 = 1, .arg1 = 2, .arg2 = 3};
    puts("Executing call from machine mode");
    xlen_t ret = bm_priv_execute_call(args);
    printf("Got %" BM_FMT_XLEN_T ", expected %" BM_FMT_XLEN_T "\n\n",
           ret,
           (args.arg0 + args.arg1 + args.arg2));

    // Enter user mode
    uint8_t *stack = u_stack + sizeof(u_stack);
    bm_priv_enter_mode(BM_PRIV_MODE_USER, entry_user, stack);
}

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/interrupt.h>
#include <baremetal/pmp.h>
#include <baremetal/priv.h>
#include <stdio.h>
#include <stdlib.h>

xlen_t call_function(xlen_t arg0, xlen_t arg1, xlen_t arg2)
{
    printf("Parameters: %llx %llx %llx\n",
           (unsigned long long)arg0,
           (unsigned long long)arg1,
           (unsigned long long)arg2);

    return (arg0 + arg1 + arg2);
}

void user_ecall_handler(void)
{
    puts("Handling syscall from user mode.");

    volatile bm_register_file_t *regs = &bm_priv_regs[bm_get_priv_mode()];
    regs->a0                          = call_function(regs->a0, regs->a1, regs->a2);

    bm_csr_write(BM_CSR_MEPC, bm_csr_read(BM_CSR_MEPC) + 0x4);
}

void machine_ecall_handler(void)
{
    puts("Handling syscall from machine mode.");

    volatile bm_register_file_t *regs = &bm_priv_regs[bm_get_priv_mode()];
    regs->a0                          = call_function(regs->a0, regs->a1, regs->a2);

    bm_csr_write(BM_CSR_MEPC, bm_csr_read(BM_CSR_MEPC) + 0x4);
}

uint8_t u_stack[0x4000] __attribute__((aligned(16)));

void __attribute__((noinline)) entry_user(void)
{
    puts("\nHello world from user mode!\n");

    bm_priv_call_args args = {.arg0 = 4, .arg1 = 5, .arg2 = 6};
    puts("Executing call from user mode");
    xlen_t ret = bm_priv_execute_call(args);
    printf("Got %llu, expected %llu\n\n",
           (unsigned long long)ret,
           (unsigned long long)(args.arg0 + args.arg1 + args.arg2));

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
    printf("Got %llu, expected %llu\n\n",
           (unsigned long long)ret,
           (unsigned long long)(args.arg0 + args.arg1 + args.arg2));

    // Enter user mode
    xlen_t stack = (xlen_t)(u_stack + sizeof(u_stack));
    bm_priv_enter_mode(BM_PRIV_MODE_USER, (xlen_t)entry_user, stack);
}

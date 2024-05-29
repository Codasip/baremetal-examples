/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/pmp.h>
#include <baremetal/priv.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t u_stack[0x4000] __attribute__((aligned(16)));

/**
 * \brief Function executed in user mode
 */
void __attribute__((noinline)) entry_user(void)
{
    puts("Hello world from user mode!");

    puts("Bye.");
    exit(EXIT_SUCCESS);
}

#ifdef TARGET_EXT_S
uint8_t s_stack[0x4000] __attribute__((aligned(16)));

/**
 * \brief Function executed in supervisor mode
 */
void __attribute__((noinline)) entry_supervisor(void)
{
    puts("Hello world from supervisor mode!");

    // Enter user mode
    xlen_t stack = (xlen_t)(u_stack + sizeof(u_stack));
    bm_priv_enter_mode(BM_PRIV_MODE_USER, (xlen_t)entry_user, stack);
}
#endif

int main(void)
{
    puts("Welcome to the privilege-drop demo!\n");

    puts("Hello world from machine mode!");

#ifdef TARGET_HAS_PMP
    // Setup PMP to allow execution and peripheral access in U/S modes.
    bm_pmp_set(0, 0xffffffff, BM_PMP_MODE_TOR, BM_PMP_CONFIG_READ | BM_PMP_MODE_WRITE | BM_PMP_MODE_EXECUTE);
#endif

#ifdef TARGET_EXT_S
    // Enter supervisor mode
    xlen_t stack = (xlen_t)(s_stack + sizeof(s_stack));
    bm_priv_enter_mode(BM_PRIV_MODE_SUPERVISOR, (xlen_t)entry_supervisor, stack);
#else
    // Enter user mode
    xlen_t stack = (xlen_t)(u_stack + sizeof(u_stack));
    bm_priv_enter_mode(BM_PRIV_MODE_USER, (xlen_t)entry_user, stack);
#endif
}

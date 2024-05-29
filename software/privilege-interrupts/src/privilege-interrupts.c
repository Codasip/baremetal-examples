/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/clint.h>
#include <baremetal/common.h>
#include <baremetal/interrupt.h>
#include <baremetal/mp.h>
#include <baremetal/platform.h>
#include <baremetal/pmp.h>
#include <baremetal/priv.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t s_stack[0x4000] __attribute__((aligned(16)));
uint8_t u_stack[0x4000] __attribute__((aligned(16)));

volatile bool stop_flag = false;

static bm_clint_t *clint;
static unsigned    clint_tics_in_second;

/**
 * \brief Handler for the MTIP (timer) interrupt
 */
void mtip_handler(void)
{
    if (stop_flag)
    {
        // Stop generation of further interrupts
        bm_interrupt_disable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MTIP);
        return;
    }

    static bool pass = false;

    bm_clint_rearm_timer(clint, bm_get_hartid(), clint_tics_in_second);

    if (!pass)
    {
        puts("MTIP handler enterred.");
    }
    else
    {
        // Pass every second interrupt to the supervisor handler in machine mode
        bm_csr_set_mask(BM_CSR_MIP, 1 << BM_INTERRUPT_SSIP);
    }

    pass = !pass;
}

/**
 * \brief Handler for the SSIP (supervisor software) interrupt
 */
void ssip_handler(void)
{
    puts("SSIP handler enterred.");

    // Clear the interrupt
    bm_csr_clear_mask(BM_CSR_MIP, 1 << BM_INTERRUPT_SSIP);
}

/**
 * \brief Function executed in user mode
 */
void __attribute__((noinline)) entry_user(void)
{
    puts("Hello world from user mode!");

    // Loop here for a while
    for (int i = 0; i < 10; ++i)
    {
        bm_delay_ms(250);
        puts("Cycling in user mode.");
    }

    stop_flag = true;
    puts("Bye.");
    exit(EXIT_SUCCESS);
}

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

int main(void)
{
    puts("Welcome to the privilege-interrupts demo!\n");

    clint                = (bm_clint_t *)target_peripheral_get(BM_PERIPHERAL_CLINT);
    clint_tics_in_second = bm_clint_ms_to_ticks(clint, 1000);

    // Start timer to trigger an interrupt every second
    bm_clint_arm_timer(clint, bm_get_hartid(), clint_tics_in_second);

    // Initialize interrupt handling for machine timer interrupt and supervisor software interrupt
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);
    bm_interrupt_set_handler(BM_INTERRUPT_MTIP, mtip_handler);
    bm_interrupt_set_handler(BM_INTERRUPT_SSIP, ssip_handler);
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MTIP);
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_SSIP);

    // Enable the cycle counter in user mode
    bm_counter_enable_priv(BM_PRIV_MODE_USER, BM_COUNTER_CYCLE);

#ifdef TARGET_HAS_PMP
    // Setup PMP to allow execution and peripheral access in U/S modes.
    bm_pmp_set(0, 0xffffffff, BM_PMP_MODE_TOR, BM_PMP_CONFIG_READ | BM_PMP_MODE_WRITE | BM_PMP_MODE_EXECUTE);
#endif

    // Enter supervisor mode
    xlen_t stack = (xlen_t)(s_stack + sizeof(s_stack));
    bm_priv_enter_mode(BM_PRIV_MODE_SUPERVISOR, (xlen_t)entry_supervisor, stack);
}

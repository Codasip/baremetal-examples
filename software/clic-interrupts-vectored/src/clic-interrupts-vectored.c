/* Copyright 2024-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/bm_cheri.h>
#include <baremetal/clic.h>
#include <baremetal/csr.h>
#include <baremetal/gpio.h>
#include <baremetal/interrupt_low.h>
#include <baremetal/mem_barrier.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bm_gpio_t *gpio0;
static bm_gpio_t *gpio1;
static bm_clic_t *clic;

#ifdef __CHERI_PURE_CAPABILITY__
/* For Cheri the vector table entries are not capabilities, they are just XLEN integer addresses.
 * The capability is created by the Cheri CLIC when an interrupt is taken.
 * From the documentation:
 *   When vectoring interrupts through the Trap Vector Table, the entry selected will be at location
 *   TBASE + (XLEN/8)*exccode, where TBASE is the address in Xtvtc and exccode is the interrupt id
 *   that will be reported in Xcause.
 *   Once the entry has been fetched, the ISR’s entry point capability is installed into PCC.
 *   This is formed by taking the address tvtentry.ENTRYPOINT and applying scaddr (Capability Set
 *   Address) instruction semantics to the capability held in the Xtvtentryic CSR selected
 *   by tvtentry.SELECTC.
 *
 * You can further bound all Interrupt Service Routine (ISR) entry points present in
 * the Trap Vector Table by writing a capability to CSR Xtvtentryic, e.g. mtvtentry0c for machine
 * mode. But you will need to write an infinate cap to Xtvtentryic if you want to
 * run your ISR in Cheri cap mode (as it defaults to the integer infinate cap).
 */

static xlen_t mtvt_table[TARGET_CLIC_NUM_INPUTS] __attribute__((aligned(64))) = {0};

#else
static void (*mtvt_table[TARGET_CLIC_NUM_INPUTS])(void) __attribute__((aligned(64))) = {0};
#endif

void __attribute__((interrupt("machine"), aligned(64))) gpio0_handler(void)
{
    puts("=== Handling GPIO INTERRUPT 0 ===");
    bm_gpio_clear_irq(gpio0);
}

void __attribute__((interrupt("machine"), aligned(64))) gpio1_handler(void)
{
    puts("=== Handling GPIO INTERRUPT 1 ===");
    bm_gpio_clear_irq(gpio1);
}

int main(void)
{
    puts("Welcome to the CLIC vectored interrupts demo!\n");

    gpio0 = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);
    gpio1 = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_SD);
    clic  = (bm_clic_t *)target_peripheral_get(BM_PERIPHERAL_CLIC);

    bm_clic_init(clic);
    bm_gpio_init_irq(gpio0);
    bm_gpio_init_irq(gpio1);

    // Setup interrupt handlers
#ifdef __CHERI_PURE_CAPABILITY__
    mtvt_table[bm_clic_get_ext_irq_id(gpio0->ext_irq_id)] = (xlen_t)gpio0_handler;
    mtvt_table[bm_clic_get_ext_irq_id(gpio1->ext_irq_id)] = (xlen_t)gpio1_handler;

    BM_CSR_WRITE_CAP2(0x307, addr_to_code_ptr((uintptr_t)mtvt_table));

    __asm__ volatile( // "csrrc	ca0, ddc, zero\n" // Read  infinite cap from ddc (here for reference)
        "auipc  ca0, 0x0\n"       // Read  infinite cap from PC
        "csrrw   x0, 0x7f8, a0\n" // Write infinite cap to mtvtentry0c
        "csrrw   x0, 0x7f9, x0\n" // Write null     cap to mtvtentry1c
        ::
            : "ca0");

#else
    xlen_t val = (xlen_t)mtvt_table;

    mtvt_table[bm_clic_get_ext_irq_id(gpio0->ext_irq_id)] = gpio0_handler;
    mtvt_table[bm_clic_get_ext_irq_id(gpio1->ext_irq_id)] = gpio1_handler;

    BM_CSR_WRITE(BM_CSR_MTVT, val);
#endif

    bm_exec_fence_i();

    // Enable external interrupts
    bm_clic_set_enable(clic, bm_clic_get_ext_irq_id(gpio0->ext_irq_id), 1);
    bm_clic_set_enable(clic, bm_clic_get_ext_irq_id(gpio1->ext_irq_id), 1);
    bm_clic_set_vectored(clic, bm_clic_get_ext_irq_id(gpio0->ext_irq_id), 1);
    bm_clic_set_vectored(clic, bm_clic_get_ext_irq_id(gpio1->ext_irq_id), 1);
    bm_interrupt_enable(BM_PRIV_MODE_MACHINE);

    // Fire interrupts
    bm_gpio_set_irq(gpio0);
    bm_gpio_set_irq(gpio1);

    puts("Bye.");
    return EXIT_SUCCESS;
}

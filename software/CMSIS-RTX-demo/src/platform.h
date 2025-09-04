/* Copyright (c) 2024-2025 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* This file contains information required for Codasip's RISC-V ports of CMSIS_6 and CMSIS-RTX:
 *
 * CMSIS_6/CMSIS/Core/Include/codasip_clic.h
 * CMSIS_6/CMSIS/Core/Source/codasip_clic.c
 * CMSIS_6/CMSIS/Core/Include/core_rv32_clic.h
 * CMSIS_6/CMSIS/RTOS2/Source/os_tick_rv32_clic.c
 *
 * CMSIS-RTX/Source/rtx_core_rv32_clic.h
 * CMSIS-RTX/Source/GCC/irq_rv32_clic.S
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "platform_info.h"

/* Include Memory Map and Interrupt Map files for platform information */
#include "interrupt_map.h"
#include "memory_map.h"

/* Set the CLIC address */
#ifndef CLIC_ADDR
    #define CLIC_ADDR 0x00040000
#endif

#define CLICCFG_ADDR (CLIC_ADDR)
#define CLICINT_ADDR (CLICCFG_ADDR + 0x1000)

/* Set the ACLINT address */
#ifndef ACLINT_BASE_ADDR
    #define ACLINT_BASE_ADDR ACLINT_ADDR
#endif

#define ACLINT_MSIP_ADDR     ACLINT_BASE_ADDR
#define ACLINT_MTIMECMP_ADDR (ACLINT_BASE_ADDR + 0x4000)
#define ACLINT_MTIME_ADDR    (ACLINT_BASE_ADDR + 0xbff8)

/* Platform Information for FreeRTOS/RTX */
#ifndef TARGET_PLATFORM_FREQ
    #define TARGET_PLATFORM_FREQ 100000000UL /* Peripherals are clocked at 100MHz, core at 50MHz */
#endif

/* Interrupts */
#ifndef CLINT_MTIMER_IRQ
    #define CLINT_MTIMER_IRQ \
        0x01 /* CLINT mtimer Interrupt wired to CLIC IRQ 1, put in 2-digit hex format for interrupt_vectors.S */
#endif

/* CLIC Configuration */
#define CLICINTCTLBITS     CONFIG_CLIC_INTCTLBITS
#define CLIC_NUM_INTERRUPT CONFIG_CLIC_NUM_INTERRUPTS

/* Define FIXED_IRQ_HANDLERS if you have a pure Harvard architecture and so the vector table has to
 * be in the instruction memory, see interrupt_vectors.S */
// #define FIXED_IRQ_HANDLERS

/* A free unconnected IRQ that can be used by software (e.g. for RTX Pending Service Call),
 * put in 2-digit hex format for interrupt_vectors.S */
#ifndef SOFTWARE_IRQ
    #define SOFTWARE_IRQ 0x0f /* Put in 2-digit hex format for interrupt_vectors.S */
#endif

/* Definitions to create IRQ Handler names froms #defines */
#define __CAT2(arg1, arg2) arg1##arg2
#define CAT2(arg1, arg2)   __CAT2(arg1, arg2)

#define IRQ_HANDLER(irq) \
    CAT2(irq_handler_, irq) /* This is used to create the ISR function name used for interrupt_vectors.S */

/* Setup the UART interrupt ID */
#define CLIC_ADJUST      2 /* The interrupt map has shifted down by 2 interrupts when using the CLIC */
#define UART_CLIC_IRQ_ID (UART_IRQ_ID + CLIC_ADJUST)

#endif /* !__PLATFORM__ */

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "core_info.h"

#define ABORT_EXIT_CODE     134
#define NMI_EXIT_CODE       135
#define TRAP_EXIT_CODE      136

.section .crt0, "ax"
_start:
    .global _start

    .option push
    .option norvc

    j _reset_handler
    j _nmi_handler

#ifdef TARGET_SIMULATION
    // Codasip simulators implement syscalls by writing to
    // codasip_syscall address at 0x10 offset from the boot address
    unimp
    unimp

.global codasip_syscall
codasip_syscall:
    .dword 0
#endif

    .option pop

_reset_handler:
    .global _reset_handler
    // Perform core-specific initialization
    jal _core_init

    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    la t0, _trap_handler
    csrw mtvec, t0

    csrr t0, mhartid
    bnez t0, _code_start
    jal clear_bss
    jal init_ram

_code_start:
    la sp, _stack

#if (TARGET_NUM_HARTS > 1)
    csrr t0, mhartid
    beqz t0, _call_main

    // Assign separate stack to each hart.
    la t1, __STACK_SIZE
    mul t1, t0, t1   // t1 <- hart's stack offset
    sub sp, sp, t1

    jal bm_park_hart
#endif

_call_main:
    // Now we can call the main function.
    lui t0, %hi(main)
    jalr ra, t0, %lo(main)
    j _exit

_nmi_handler:
    .global _nmi_handler
    li a0, NMI_EXIT_CODE
    j _exit

.balign 4
_trap_handler:
    .global _trap_handler
    li a0, TRAP_EXIT_CODE
    j _exit

abort:
    .global abort
    li a0, ABORT_EXIT_CODE
    j _exit

exit:
    .global exit
    j _exit

clear_bss:
    .global clear_bss
#ifndef TARGET_SIMULATION
    // not needed in sim, can be slow
    la a0, __bss_start
    la a1, __bss_end
1:
    sw zero, 0(a0)
    addi a0, a0, 4
    blt a0, a1, 1b
#endif
    ret

init_ram:
    .weak init_ram
    ret

bm_park_hart:
    .weak bm_park_hart
    j bm_park_hart

_core_init:
    .weak _core_init
    ret

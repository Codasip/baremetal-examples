/*
 * Origional version:
 * Copyright (c) 2019, The Regents of the University of California (Regents).
 * All Rights Reserved.
 * CHERI and libgloss-hobgoblin changes:
 * Copyright (c) 2025-2026 Codasip Limited. All rights reserved.
 */

#include "asm.h"
#include "encoding.h"

#ifdef __CHERI_PURE_CAPABILITY__
.section .data
.balign __SIZEOF_CHERI_CAPABILITY__
.global g_inf_cap
g_inf_cap:
    .chericap 0
    .size g_inf_cap, __SIZEOF_CHERI_CAPABILITY__
#endif

    .section .text.init
init_ram:
    .weak init_ram
    ret

    .global _start_libgloss
    .type _start_libgloss, @function
_start_libgloss:
    .cfi_startproc

    li x1, 0
    li x2, 0
    li x3, 0
    li x4, 0
    li x5, 0
    li x6, 0
    li x7, 0
    li x8, 0
    li x9, 0
    li x10, 0
    li x11, 0
    li x12, 0
    li x13, 0
    li x14, 0
    li x15, 0
    li x16, 0
    li x17, 0
    li x18, 0
    li x19, 0
    li x20, 0
    li x21, 0
    li x22, 0
    li x23, 0
    li x24, 0
    li x25, 0
    li x26, 0
    li x27, 0
    li x28, 0
    li x29, 0
    li x30, 0
    li x31, 0

    li t0, (MSTATUS_FS | MSTATUS_XS | MSTATUS_VS)
    csrs mstatus, t0

#ifdef __riscv_flen
    /* Initialize FPU if present */
    csrr t0, mstatus
    li t1, MSTATUS_FS
    and t0, t0, t1
    beqz t0, 1f

    fscsr zero
    fmv.s.x f0,  zero
    fmv.s.x f1,  zero
    fmv.s.x f2,  zero
    fmv.s.x f3,  zero
    fmv.s.x f4,  zero
    fmv.s.x f5,  zero
    fmv.s.x f6,  zero
    fmv.s.x f7,  zero
    fmv.s.x f8,  zero
    fmv.s.x f9,  zero
    fmv.s.x f10, zero
    fmv.s.x f11, zero
    fmv.s.x f12, zero
    fmv.s.x f13, zero
    fmv.s.x f14, zero
    fmv.s.x f15, zero
    fmv.s.x f16, zero
    fmv.s.x f17, zero
    fmv.s.x f18, zero
    fmv.s.x f19, zero
    fmv.s.x f20, zero
    fmv.s.x f21, zero
    fmv.s.x f22, zero
    fmv.s.x f23, zero
    fmv.s.x f24, zero
    fmv.s.x f25, zero
    fmv.s.x f26, zero
    fmv.s.x f27, zero
    fmv.s.x f28, zero
    fmv.s.x f29, zero
    fmv.s.x f30, zero
    fmv.s.x f31, zero
1:
#endif

    /* Initialize global pointer */
    .option push
#ifdef __CHERI_PURE_CAPABILITY__
    .option nocapmode
    # purecap mode does not use gp yet
#else
    /*
     * Disable relaxation to avoid relaxing the address calculation to
     * "addi gp, gp, 0"
     */
    .option norelax
    la gp, __global_pointer$
#endif
    .option pop

/*
 * Jump into purecap CHERI mode if required
 */
#ifdef __CHERI_PURE_CAPABILITY__
    .option push
    .option nocapmode
#if defined(__riscv_zcherihybrid)
    /* Switch to purecap mode if it's not in purecap mode already */
    csrs mseccfg, (1 << 3) # mseccfg.cre = Enabled
    modesw.cap
#else
#error Not supported yet
#endif
    .option pop
#endif /* __CHERI_PURE_CAPABILITY__ */

    /* Initialise .data section with data in rom, if required */
    call    init_ram

    /* Initialize thread pointer */
    csrr s0, mhartid
    lui t0, %hi(__stack_shift)
    addi t0, t0, %lo(__stack_shift)
    sll t0, s0, t0
#ifdef __CHERI_PURE_CAPABILITY__
    llc ctp, __stack_start
    cadd ctp, ctp, t0
#else
    la tp, __stack_start
    add tp, tp, t0
#endif

    /* Initialize stack pointer */
    lui t0, %hi(__stack_size)
    addi t0, t0, %lo(__stack_size)
#ifdef __CHERI_PURE_CAPABILITY__
    scbnds csp, ctp, t0          // Set the bounds from bottom to top
    li t2, ~__CHERI_CAP_PERMISSION_EXECUTE__
    acperm csp, csp, t2        // No execute on stack
    cadd csp, csp, t0
#else
    add sp, tp, t0
#endif

    /* Initialize TLS */
#ifdef __CHERI_PURE_CAPABILITY__
    cmv ca0, ctp
#else
    mv a0, tp
#endif
    call __init_tls

    /* Skip global initialization if not the designated boot hart */
#ifdef __CHERI_PURE_CAPABILITY__
    llc ct0, __boot_hart
#else
    la t0, __boot_hart
#endif
    bne s0, t0, _start_secondary

    /* Zero BSS segment */
#ifdef __CHERI_PURE_CAPABILITY__
    llc     ca0, __bss_start
    llc     ca2, __bss_end
    sub     a2, a2, a0
    scbndsr ca0, ca0, a2
#else /* !__CHERI_PURE_CAPABILITY__ */
    la      a0, __bss_start
    la      a2, __bss_end
    sub     a2, a2, a0
#endif /* __CHERI_PURE_CAPABILITY__ */
    li      a1, 0
    call    memset

  /*
   * Initialise capabilities for C code
   */
#if __has_feature(capabilities)
#ifdef __CHERI_PURE_CAPABILITY__
    # Initialize capabilities

    # Create the infinate cap in ca0
    auipc ct1, 0
    scaddr ct1, ct1, zero
    cmv ca0, ct1

    # Store the infinate cap
    llc     ct1, g_inf_cap
    sc      ca0, 0(ct1)

    # Initialize capabilities
    llc    ct0, init_cap_globals // defined in file cheri_init.c
    jalr   ct0

#if __SIZEOF_CHERI_CAPABILITY__ >= 16
    # remove data permissions from PCC
    # Can only remove WRITE, it still needs to read capabilities
    # On rv32 cannot remove WRITE as that will also remove ASR
    # and we need to be able to access system registers in machine
    # mode
    llc    ct0, 1f
    li     t1,  ~__CHERI_CAP_PERMISSION_WRITE__
    acperm ct0, ct0, t1
    jr     ct0
1:
    # Note that from here onwards, llc cannot be used to create a
    # capability with write permission
#endif

    # remove execute permissions from DDC
    csrr   ct0, ddc
    li     t1,  ~__CHERI_CAP_PERMISSION_EXECUTE__
    acperm ct0, ct0, t1
    csrw   ddc, ct0

#else /* __CHERI_PURE_CAPABILITY__ */
#error Not supported yet
#endif /* __CHERI_PURE_CAPABILITY__ */
#endif /* __has_feature(capabilities) */

    /*
     * Set up the trap handler
     */
#ifdef __CHERI_PURE_CAPABILITY__
    llc ct0, __traphandler
    csrw mtvecc, ct0
#else /* __CHERI_PURE_CAPABILITY__ */
    la t0, __traphandler
    csrw mtvec, t0
#endif /* __CHERI_PURE_CAPABILITY__ */

    /*
     * Initialize the capabiliry for the heap
     */
#ifdef __CHERI_PURE_CAPABILITY__
    llc ct0, __heap_start
    csrr ct1, ddc
    scaddr ct0, ct1, t0
    llc ct1, __heap_end
    sub t1, t1, t0
    scbndsr ct0, ct0, t1
    li t2, ~__CHERI_CAP_PERMISSION_EXECUTE__
    acperm ct0, ct0, t2         // no execute on heap
    lgc ct1, HeapCap            // Cannot use llc, cannot write through pcc
    sc ct0, 0(ct1)              // store in HeapCap
#endif /* __CHERI_PURE_CAPABILITY__ */

    /* Set up call to global destructors on exit */
#ifdef _LITE_EXIT
    # Make reference to atexit weak to avoid unconditionally pulling in
    # support code.  Refer to comments in __atexit.c for more details.
    .weak   atexit
#ifdef __CHERI_PURE_CAPABILITY__
    llc     ca0, atexit
#else /* __CHERI_PURE_CAPABILITY__ */
    la      a0, atexit
#endif /* __CHERI_PURE_CAPABILITY__ */
    beqz    a0, .Lweak_atexit
    .weak   __libc_fini_array
#endif /* _LITE_EXIT */

#ifdef __CHERI_PURE_CAPABILITY__
    llc     ca0, __libc_fini_array   # Register global termination functions
    llc     ct0, atexit
    jalr    ct0
#else /* __CHERI_PURE_CAPABILITY__ */
    la      a0, __libc_fini_array    # Register global termination functions
    call    atexit                   #  to be called upon exit
#endif /* __CHERI_PURE_CAPABILITY__ */

#ifdef _LITE_EXIT
.Lweak_atexit:
#endif

    /* Call global constructors */
#ifdef __CHERI_PURE_CAPABILITY__
    llc     ct0, __libc_init_array
    jalr    ct0                      # Run global initialization functions
#else
    call    __libc_init_array       # Run global initialization functions
#endif

    /* Call main function */
    tail _start_main

    .cfi_endproc

#if defined(__CHERI_PURE_CAPABILITY__)
    .bss
    .global HeapCap
    .type   ptr,@object
    .balign 16
HeapCap:
    .chericap 0
    .size   HeapCap, .-HeapCap
#endif /* __CHERI_PURE_CAPABILITY__ */

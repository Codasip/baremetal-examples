/* Copyright 2026 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */
   
    .section .preserve.text, "ax", @progbits
    .globl execute_jump
    .type  execute_jump, @function

    # C signature:
    # void execute_jump(uint32_t hart_id,
    #                               void *dtb,
    #                               fw_dynamic_info_t *fw_dynamic_info_ptr,
    #                               payload_func_t func_ptr)

.macro CALL rs1
#ifdef __CHERI_PURE_CAPABILITY__
    jalr c\rs1
#else
    jalr \rs1
#endif
.endm

execute_jump:
#ifdef __CHERI_PURE_CAPABILITY__
    lgc ct0, g_inf_cap
    lc  ct0, 0(ct0)
#ifdef __riscv_zcherihybrid
    li t1, 1                  
    scmode ct0, ct0, t1    #Set ct0 (our infinite cap) to an integer mode capability
    scmode ca3, ca3, t1    #Set ca3 (our target) to an integer mode capability
    csrw ddc, ct0          #Reset DDC as integer mode infinite cap
#endif
    csrw mtvecc, ct0       #Reset mtvecc as integer mode infinite cap    

#    Not required (see CALL below):
#    csrc mseccfg, (1 << 3) # mseccfg.cre = Disabled
#    modesw.int
#endif  

    /* Switch to integer mode via JARL to integer cap */
    CALL a3
    ret                       # if it returns, return to the C wrapper

    .size execute_jump, .-execute_jump

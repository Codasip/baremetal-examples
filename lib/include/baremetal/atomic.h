/* Copyright 2026 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_ATOMIC_H
#define BAREMETAL_ATOMIC_H

#ifdef __riscv_atomic

    /* Atomic Memory Operations */
    #ifdef __CHERI_PURE_CAPABILITY__
        #define AMO(instr, rd, rs2, rs1) \
            __asm__ volatile(#instr " %0, %1, (%2)\n" : "=r"(rd) : "r"(rs2), "C"(rs1) : "memory");

    #else
        #define AMO(instr, rd, rs2, rs1) \
            __asm__ volatile(#instr " %0, %1, (%2)\n" : "=r"(rd) : "r"(rs2), "r"(rs1) : "memory");
    #endif

    #define bm_lr_w(rd, rs1)           AMO(lr.w, rd, x0, rs1)
    #define bm_sc_w(rd, rs2, rs1)      AMO(sc.w, rd, rs2, rs1)

    #define bm_amoswap_w(rd, rs2, rs1) AMO(amoswap.w, rd, rs2, rs1)
    #define bm_amoadd_w(rd, rs2, rs1)  AMO(amoadd.w, rd, rs2, rs1)
    #define bm_amoxor_w(rd, rs2, rs1)  AMO(amoxor.w, rd, rs2, rs1)
    #define bm_amoand_w(rd, rs2, rs1)  AMO(amoand.w, rd, rs2, rs1)
    #define bm_amoor_w(rd, rs2, rs1)   AMO(amoor.w, rd, rs2, rs1)
    #define bm_amomin_w(rd, rs2, rs1)  AMO(amomin.w, rd, rs2, rs1)
    #define bm_amomax_w(rd, rs2, rs1)  AMO(amomax.w, rd, rs2, rs1)
    #define bm_amominu_w(rd, rs2, rs1) AMO(amominu.w, rd, rs2, rs1)
    #define bm_amomaxu_w(rd, rs2, rs1) AMO(amomaxu.w, rd, rs2, rs1)

    #define bm_lr_d(rd, rs1)           AMO(lr.d, rd, x0, rs1)
    #define bm_sc_d(rd, rs2, rs1)      AMO(sc.d, rd, rs2, rs1)

    #define bm_amoswap_d(rd, rs2, rs1) AMO(amoswap.d, rd, rs2, rs1)
    #define bm_amoadd_d(rd, rs2, rs1)  AMO(amoadd.d, rd, rs2, rs1)
    #define bm_amoxor_d(rd, rs2, rs1)  AMO(amoxor.d, rd, rs2, rs1)
    #define bm_amoand_d(rd, rs2, rs1)  AMO(amoand.d, rd, rs2, rs1)
    #define bm_amoor_d(rd, rs2, rs1)   AMO(amoor.d, rd, rs2, rs1)
    #define bm_amomin_d(rd, rs2, rs1)  AMO(amomin.d, rd, rs2, rs1)
    #define bm_amomax_d(rd, rs2, rs1)  AMO(amomax.d, rd, rs2, rs1)
    #define bm_amominu_d(rd, rs2, rs1) AMO(amominu.d, rd, rs2, rs1)
    #define bm_amomaxu_d(rd, rs2, rs1) AMO(amomaxu.d, rd, rs2, rs1)

#endif /* __riscv_atomic */

#endif /* !BAREMETAL_ATOMIC_H */

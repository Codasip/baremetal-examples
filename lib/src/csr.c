/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/csr.h"

#include "baremetal/common.h"
#include "baremetal/verbose.h"

// Helper macros for PMP config registers
#define TARGET_HAS_PMP_CFG_REG1 ((TARGET_PMP_NUM_REGIONS > 4) && (__riscv_xlen == 32))
#define TARGET_HAS_PMP_CFG_REG2 (TARGET_PMP_NUM_REGIONS > 8)
#define TARGET_HAS_PMP_CFG_REG3 ((TARGET_PMP_NUM_REGIONS > 12) && (__riscv_xlen == 32))

xlen_t bm_csr_read(const int csr)
{
    xlen_t value = 0;

    switch (csr)
    {
#ifdef TARGET_EXT_N
        case BM_CSR_USTATUS:
            CSR_READ(BM_CSR_USTATUS, value);
            break;
#endif
#ifdef __riscv_flen
        case BM_CSR_FFLAGS:
            CSR_READ(BM_CSR_FFLAGS, value);
            break;
        case BM_CSR_FRM:
            CSR_READ(BM_CSR_FRM, value);
            break;
        case BM_CSR_FCSR:
            CSR_READ(BM_CSR_FCSR, value);
            break;
#endif
#ifdef TARGET_EXT_N
        case BM_CSR_UIE:
            CSR_READ(BM_CSR_UIE, value);
            break;
        case BM_CSR_UTVEC:
            CSR_READ(BM_CSR_UTVEC, value);
            break;
        case BM_CSR_USCRATCH:
            CSR_READ(BM_CSR_USCRATCH, value);
            break;
        case BM_CSR_UEPC:
            CSR_READ(BM_CSR_UEPC, value);
            break;
        case BM_CSR_UCAUSE:
            CSR_READ(BM_CSR_UCAUSE, value);
            break;
        case BM_CSR_UTVAL:
            CSR_READ(BM_CSR_UTVAL, value);
            break;
        case BM_CSR_UIP:
            CSR_READ(BM_CSR_UIP, value);
            break;
#endif
        case BM_CSR_SEED:
            CSR_READ(BM_CSR_SEED, value);
            break;
#ifdef TARGET_EXT_S
        case BM_CSR_SSTATUS:
            CSR_READ(BM_CSR_SSTATUS, value);
            break;
    #ifdef TARGET_EXT_N
        case BM_CSR_SEDELEG:
            CSR_READ(BM_CSR_SEDELEG, value);
            break;
        case BM_CSR_SIDELEG:
            CSR_READ(BM_CSR_SIDELEG, value);
            break;
    #endif
        case BM_CSR_SIE:
            CSR_READ(BM_CSR_SIE, value);
            break;
        case BM_CSR_STVEC:
            CSR_READ(BM_CSR_STVEC, value);
            break;
        case BM_CSR_SCOUNTEREN:
            CSR_READ(BM_CSR_SCOUNTEREN, value);
            break;
        case BM_CSR_SSCRATCH:
            CSR_READ(BM_CSR_SSCRATCH, value);
            break;
        case BM_CSR_SEPC:
            CSR_READ(BM_CSR_SEPC, value);
            break;
        case BM_CSR_SCAUSE:
            CSR_READ(BM_CSR_SCAUSE, value);
            break;
        case BM_CSR_STVAL:
            CSR_READ(BM_CSR_STVAL, value);
            break;
        case BM_CSR_SIP:
            CSR_READ(BM_CSR_SIP, value);
            break;
        case BM_CSR_SATP:
            CSR_READ(BM_CSR_SATP, value);
            break;
    #ifdef TARGET_HAS_CLIC
        case BM_CSR_STVT:
            CSR_READ(BM_CSR_STVT, value);
            break;
        case BM_CSR_SNXTI:
            CSR_READ(BM_CSR_SNXTI, value);
            break;
        case BM_CSR_SINTTHRESH:
            CSR_READ(BM_CSR_SINTTHRESH, value);
            break;
        case BM_CSR_SSCRATCHCSW:
            CSR_READ(BM_CSR_SSCRATCHCSW, value);
            break;
        case BM_CSR_SSCRATCHCSWL:
            CSR_READ(BM_CSR_SSCRATCHCSWL, value);
            break;
        case BM_CSR_SINTSTATUS:
            CSR_READ(BM_CSR_SINTSTATUS, value);
            break;
    #endif
        case BM_CSR_SENVCFG:
            CSR_READ(BM_CSR_SENVCFG, value);
            break;
#endif
        case BM_CSR_MSTATUS:
            CSR_READ(BM_CSR_MSTATUS, value);
            break;
        case BM_CSR_MISA:
            CSR_READ(BM_CSR_MISA, value);
            break;
#if defined(TARGET_EXT_S) || defined(TARGET_EXT_N)
        case BM_CSR_MEDELEG:
            CSR_READ(BM_CSR_MEDELEG, value);
            break;
        case BM_CSR_MIDELEG:
            CSR_READ(BM_CSR_MIDELEG, value);
            break;
#endif
        case BM_CSR_MIE:
            CSR_READ(BM_CSR_MIE, value);
            break;
        case BM_CSR_MTVEC:
            CSR_READ(BM_CSR_MTVEC, value);
            break;
#ifdef TARGET_HAS_CLIC
        case BM_CSR_MTVT:
            CSR_READ(BM_CSR_MTVT, value);
            break;
        case BM_CSR_MNXTI:
            CSR_READ(BM_CSR_MNXTI, value);
            break;
        case BM_CSR_MINTTHRESH:
            CSR_READ(BM_CSR_MINTTHRESH, value);
            break;
        case BM_CSR_MSCRATCHCSW:
            CSR_READ(BM_CSR_MSCRATCHCSW, value);
            break;
        case BM_CSR_MSCRATCHCSWL:
            CSR_READ(BM_CSR_MSCRATCHCSWL, value);
            break;
        case BM_CSR_MINTSTATUS:
            CSR_READ(BM_CSR_MINTSTATUS, value);
            break;
#endif
        case BM_CSR_MENVCFG:
            CSR_READ(BM_CSR_MENVCFG, value);
            break;
#if __riscv_xlen == 32
        case BM_CSR_MENVCFGH:
            CSR_READ(BM_CSR_MENVCFGH, value);
            break;
#endif
        case BM_CSR_MCOUNTEREN:
            CSR_READ(BM_CSR_MCOUNTEREN, value);
            break;
        case BM_CSR_MCOUNTINHIBIT:
            CSR_READ(BM_CSR_MCOUNTINHIBIT, value);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMEVENT3:
            CSR_READ(BM_CSR_MHPMEVENT3, value);
            break;
        case BM_CSR_MHPMEVENT4:
            CSR_READ(BM_CSR_MHPMEVENT4, value);
            break;
        case BM_CSR_MHPMEVENT5:
            CSR_READ(BM_CSR_MHPMEVENT5, value);
            break;
        case BM_CSR_MHPMEVENT6:
            CSR_READ(BM_CSR_MHPMEVENT6, value);
            break;
        case BM_CSR_MHPMEVENT7:
            CSR_READ(BM_CSR_MHPMEVENT7, value);
            break;
        case BM_CSR_MHPMEVENT8:
            CSR_READ(BM_CSR_MHPMEVENT8, value);
            break;
        case BM_CSR_MHPMEVENT9:
            CSR_READ(BM_CSR_MHPMEVENT9, value);
            break;
        case BM_CSR_MHPMEVENT10:
            CSR_READ(BM_CSR_MHPMEVENT10, value);
            break;
        case BM_CSR_MHPMEVENT11:
            CSR_READ(BM_CSR_MHPMEVENT11, value);
            break;
        case BM_CSR_MHPMEVENT12:
            CSR_READ(BM_CSR_MHPMEVENT12, value);
            break;
        case BM_CSR_MHPMEVENT13:
            CSR_READ(BM_CSR_MHPMEVENT13, value);
            break;
        case BM_CSR_MHPMEVENT14:
            CSR_READ(BM_CSR_MHPMEVENT14, value);
            break;
        case BM_CSR_MHPMEVENT15:
            CSR_READ(BM_CSR_MHPMEVENT15, value);
            break;
        case BM_CSR_MHPMEVENT16:
            CSR_READ(BM_CSR_MHPMEVENT16, value);
            break;
        case BM_CSR_MHPMEVENT17:
            CSR_READ(BM_CSR_MHPMEVENT17, value);
            break;
        case BM_CSR_MHPMEVENT18:
            CSR_READ(BM_CSR_MHPMEVENT18, value);
            break;
        case BM_CSR_MHPMEVENT19:
            CSR_READ(BM_CSR_MHPMEVENT19, value);
            break;
        case BM_CSR_MHPMEVENT20:
            CSR_READ(BM_CSR_MHPMEVENT20, value);
            break;
        case BM_CSR_MHPMEVENT21:
            CSR_READ(BM_CSR_MHPMEVENT21, value);
            break;
        case BM_CSR_MHPMEVENT22:
            CSR_READ(BM_CSR_MHPMEVENT22, value);
            break;
        case BM_CSR_MHPMEVENT23:
            CSR_READ(BM_CSR_MHPMEVENT23, value);
            break;
        case BM_CSR_MHPMEVENT24:
            CSR_READ(BM_CSR_MHPMEVENT24, value);
            break;
        case BM_CSR_MHPMEVENT25:
            CSR_READ(BM_CSR_MHPMEVENT25, value);
            break;
        case BM_CSR_MHPMEVENT26:
            CSR_READ(BM_CSR_MHPMEVENT26, value);
            break;
        case BM_CSR_MHPMEVENT27:
            CSR_READ(BM_CSR_MHPMEVENT27, value);
            break;
        case BM_CSR_MHPMEVENT28:
            CSR_READ(BM_CSR_MHPMEVENT28, value);
            break;
        case BM_CSR_MHPMEVENT29:
            CSR_READ(BM_CSR_MHPMEVENT29, value);
            break;
        case BM_CSR_MHPMEVENT30:
            CSR_READ(BM_CSR_MHPMEVENT30, value);
            break;
        case BM_CSR_MHPMEVENT31:
            CSR_READ(BM_CSR_MHPMEVENT31, value);
            break;
#endif
        case BM_CSR_MSCRATCH:
            CSR_READ(BM_CSR_MSCRATCH, value);
            break;
        case BM_CSR_MEPC:
            CSR_READ(BM_CSR_MEPC, value);
            break;
        case BM_CSR_MCAUSE:
            CSR_READ(BM_CSR_MCAUSE, value);
            break;
        case BM_CSR_MTVAL:
            CSR_READ(BM_CSR_MTVAL, value);
            break;
        case BM_CSR_MIP:
            CSR_READ(BM_CSR_MIP, value);
            break;
#ifdef TARGET_HAS_PMP
        case BM_CSR_PMPCFG0:
            CSR_READ(BM_CSR_PMPCFG0, value);
            break;
    #if TARGET_HAS_PMP_CFG_REG1
        case BM_CSR_PMPCFG1:
            CSR_READ(BM_CSR_PMPCFG1, value);
            break;
    #endif
    #if TARGET_HAS_PMP_CFG_REG2
        case BM_CSR_PMPCFG2:
            CSR_READ(BM_CSR_PMPCFG2, value);
            break;
    #endif
    #if TARGET_HAS_PMP_CFG_REG3
        case BM_CSR_PMPCFG3:
            CSR_READ(BM_CSR_PMPCFG3, value);
            break;
    #endif
        case BM_CSR_PMPADDR0:
            CSR_READ(BM_CSR_PMPADDR0, value);
            break;
        case BM_CSR_PMPADDR1:
            CSR_READ(BM_CSR_PMPADDR1, value);
            break;
        case BM_CSR_PMPADDR2:
            CSR_READ(BM_CSR_PMPADDR2, value);
            break;
        case BM_CSR_PMPADDR3:
            CSR_READ(BM_CSR_PMPADDR3, value);
            break;
    #if TARGET_PMP_NUM_REGIONS > 4
        case BM_CSR_PMPADDR4:
            CSR_READ(BM_CSR_PMPADDR4, value);
            break;
        case BM_CSR_PMPADDR5:
            CSR_READ(BM_CSR_PMPADDR5, value);
            break;
        case BM_CSR_PMPADDR6:
            CSR_READ(BM_CSR_PMPADDR6, value);
            break;
        case BM_CSR_PMPADDR7:
            CSR_READ(BM_CSR_PMPADDR7, value);
            break;
    #endif
    #if TARGET_PMP_NUM_REGIONS > 8
        case BM_CSR_PMPADDR8:
            CSR_READ(BM_CSR_PMPADDR8, value);
            break;
        case BM_CSR_PMPADDR9:
            CSR_READ(BM_CSR_PMPADDR9, value);
            break;
        case BM_CSR_PMPADDR10:
            CSR_READ(BM_CSR_PMPADDR10, value);
            break;
        case BM_CSR_PMPADDR11:
            CSR_READ(BM_CSR_PMPADDR11, value);
            break;
    #endif
    #if TARGET_PMP_NUM_REGIONS > 12
        case BM_CSR_PMPADDR12:
            CSR_READ(BM_CSR_PMPADDR12, value);
            break;
        case BM_CSR_PMPADDR13:
            CSR_READ(BM_CSR_PMPADDR13, value);
            break;
        case BM_CSR_PMPADDR14:
            CSR_READ(BM_CSR_PMPADDR14, value);
            break;
        case BM_CSR_PMPADDR15:
            CSR_READ(BM_CSR_PMPADDR15, value);
            break;
    #endif
#endif
        case BM_CSR_MSECCFG:
            CSR_READ(BM_CSR_MSECCFG, value);
            break;
#if __riscv_xlen == 32
        case BM_CSR_MSECCFGH:
            CSR_READ(BM_CSR_MSECCFGH, value);
            break;
#endif
        case BM_CSR_TSELECT:
            CSR_READ(BM_CSR_TSELECT, value);
            break;
        case BM_CSR_TDATA1:
            CSR_READ(BM_CSR_TDATA1, value);
            break;
        case BM_CSR_TDATA2:
            CSR_READ(BM_CSR_TDATA2, value);
            break;
        case BM_CSR_TDATA3:
            CSR_READ(BM_CSR_TDATA3, value);
            break;
        case BM_CSR_TINFO:
            CSR_READ(BM_CSR_TINFO, value);
            break;
        case BM_CSR_TCONTROL:
            CSR_READ(BM_CSR_TCONTROL, value);
            break;
        case BM_CSR_MCONTEXT:
            CSR_READ(BM_CSR_MCONTEXT, value);
            break;
#ifdef TARGET_HAS_PIC
        case BM_CSR_MPICFLAG:
            CSR_READ(BM_CSR_MPICFLAG, value);
            break;
        case BM_CSR_MPICMASK:
            CSR_READ(BM_CSR_MPICMASK, value);
            break;
    #if TARGET_PIC_NUM_INTERRUPTS > 32
        case BM_CSR_MPICFLAG1:
            CSR_READ(BM_CSR_MPICFLAG1, value);
            break;
        case BM_CSR_MPICMASK1:
            CSR_READ(BM_CSR_MPICMASK1, value);
            break;
    #endif
    #if TARGET_PIC_NUM_INTERRUPTS > 64
        case BM_CSR_MPICFLAG2:
            CSR_READ(BM_CSR_MPICFLAG2, value);
            break;
        case BM_CSR_MPICMASK2:
            CSR_READ(BM_CSR_MPICMASK2, value);
            break;
    #endif
    #if TARGET_PIC_NUM_INTERRUPTS > 96
        case BM_CSR_MPICFLAG3:
            CSR_READ(BM_CSR_MPICFLAG3, value);
            break;
        case BM_CSR_MPICMASK3:
            CSR_READ(BM_CSR_MPICMASK3, value);
            break;
    #endif
#endif
        case BM_CSR_MCYCLE:
            CSR_READ(BM_CSR_MCYCLE, value);
            break;
        case BM_CSR_MINSTRET:
            CSR_READ(BM_CSR_MINSTRET, value);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMCOUNTER3:
            CSR_READ(BM_CSR_MHPMCOUNTER3, value);
            break;
        case BM_CSR_MHPMCOUNTER4:
            CSR_READ(BM_CSR_MHPMCOUNTER4, value);
            break;
        case BM_CSR_MHPMCOUNTER5:
            CSR_READ(BM_CSR_MHPMCOUNTER5, value);
            break;
        case BM_CSR_MHPMCOUNTER6:
            CSR_READ(BM_CSR_MHPMCOUNTER6, value);
            break;
        case BM_CSR_MHPMCOUNTER7:
            CSR_READ(BM_CSR_MHPMCOUNTER7, value);
            break;
        case BM_CSR_MHPMCOUNTER8:
            CSR_READ(BM_CSR_MHPMCOUNTER8, value);
            break;
        case BM_CSR_MHPMCOUNTER9:
            CSR_READ(BM_CSR_MHPMCOUNTER9, value);
            break;
        case BM_CSR_MHPMCOUNTER10:
            CSR_READ(BM_CSR_MHPMCOUNTER10, value);
            break;
        case BM_CSR_MHPMCOUNTER11:
            CSR_READ(BM_CSR_MHPMCOUNTER11, value);
            break;
        case BM_CSR_MHPMCOUNTER12:
            CSR_READ(BM_CSR_MHPMCOUNTER12, value);
            break;
        case BM_CSR_MHPMCOUNTER13:
            CSR_READ(BM_CSR_MHPMCOUNTER13, value);
            break;
        case BM_CSR_MHPMCOUNTER14:
            CSR_READ(BM_CSR_MHPMCOUNTER14, value);
            break;
        case BM_CSR_MHPMCOUNTER15:
            CSR_READ(BM_CSR_MHPMCOUNTER15, value);
            break;
        case BM_CSR_MHPMCOUNTER16:
            CSR_READ(BM_CSR_MHPMCOUNTER16, value);
            break;
        case BM_CSR_MHPMCOUNTER17:
            CSR_READ(BM_CSR_MHPMCOUNTER17, value);
            break;
        case BM_CSR_MHPMCOUNTER18:
            CSR_READ(BM_CSR_MHPMCOUNTER18, value);
            break;
        case BM_CSR_MHPMCOUNTER19:
            CSR_READ(BM_CSR_MHPMCOUNTER19, value);
            break;
        case BM_CSR_MHPMCOUNTER20:
            CSR_READ(BM_CSR_MHPMCOUNTER20, value);
            break;
        case BM_CSR_MHPMCOUNTER21:
            CSR_READ(BM_CSR_MHPMCOUNTER21, value);
            break;
        case BM_CSR_MHPMCOUNTER22:
            CSR_READ(BM_CSR_MHPMCOUNTER22, value);
            break;
        case BM_CSR_MHPMCOUNTER23:
            CSR_READ(BM_CSR_MHPMCOUNTER23, value);
            break;
        case BM_CSR_MHPMCOUNTER24:
            CSR_READ(BM_CSR_MHPMCOUNTER24, value);
            break;
        case BM_CSR_MHPMCOUNTER25:
            CSR_READ(BM_CSR_MHPMCOUNTER25, value);
            break;
        case BM_CSR_MHPMCOUNTER26:
            CSR_READ(BM_CSR_MHPMCOUNTER26, value);
            break;
        case BM_CSR_MHPMCOUNTER27:
            CSR_READ(BM_CSR_MHPMCOUNTER27, value);
            break;
        case BM_CSR_MHPMCOUNTER28:
            CSR_READ(BM_CSR_MHPMCOUNTER28, value);
            break;
        case BM_CSR_MHPMCOUNTER29:
            CSR_READ(BM_CSR_MHPMCOUNTER29, value);
            break;
        case BM_CSR_MHPMCOUNTER30:
            CSR_READ(BM_CSR_MHPMCOUNTER30, value);
            break;
        case BM_CSR_MHPMCOUNTER31:
            CSR_READ(BM_CSR_MHPMCOUNTER31, value);
            break;
#endif
#if __riscv_xlen == 32
        case BM_CSR_MCYCLEH:
            CSR_READ(BM_CSR_MCYCLEH, value);
            break;
        case BM_CSR_MINSTRETH:
            CSR_READ(BM_CSR_MINSTRETH, value);
            break;
    #ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMCOUNTER3H:
            CSR_READ(BM_CSR_MHPMCOUNTER3H, value);
            break;
        case BM_CSR_MHPMCOUNTER4H:
            CSR_READ(BM_CSR_MHPMCOUNTER4H, value);
            break;
        case BM_CSR_MHPMCOUNTER5H:
            CSR_READ(BM_CSR_MHPMCOUNTER5H, value);
            break;
        case BM_CSR_MHPMCOUNTER6H:
            CSR_READ(BM_CSR_MHPMCOUNTER6H, value);
            break;
        case BM_CSR_MHPMCOUNTER7H:
            CSR_READ(BM_CSR_MHPMCOUNTER7H, value);
            break;
        case BM_CSR_MHPMCOUNTER8H:
            CSR_READ(BM_CSR_MHPMCOUNTER8H, value);
            break;
        case BM_CSR_MHPMCOUNTER9H:
            CSR_READ(BM_CSR_MHPMCOUNTER9H, value);
            break;
        case BM_CSR_MHPMCOUNTER10H:
            CSR_READ(BM_CSR_MHPMCOUNTER10H, value);
            break;
        case BM_CSR_MHPMCOUNTER11H:
            CSR_READ(BM_CSR_MHPMCOUNTER11H, value);
            break;
        case BM_CSR_MHPMCOUNTER12H:
            CSR_READ(BM_CSR_MHPMCOUNTER12H, value);
            break;
        case BM_CSR_MHPMCOUNTER13H:
            CSR_READ(BM_CSR_MHPMCOUNTER13H, value);
            break;
        case BM_CSR_MHPMCOUNTER14H:
            CSR_READ(BM_CSR_MHPMCOUNTER14H, value);
            break;
        case BM_CSR_MHPMCOUNTER15H:
            CSR_READ(BM_CSR_MHPMCOUNTER15H, value);
            break;
        case BM_CSR_MHPMCOUNTER16H:
            CSR_READ(BM_CSR_MHPMCOUNTER16H, value);
            break;
        case BM_CSR_MHPMCOUNTER17H:
            CSR_READ(BM_CSR_MHPMCOUNTER17H, value);
            break;
        case BM_CSR_MHPMCOUNTER18H:
            CSR_READ(BM_CSR_MHPMCOUNTER18H, value);
            break;
        case BM_CSR_MHPMCOUNTER19H:
            CSR_READ(BM_CSR_MHPMCOUNTER19H, value);
            break;
        case BM_CSR_MHPMCOUNTER20H:
            CSR_READ(BM_CSR_MHPMCOUNTER20H, value);
            break;
        case BM_CSR_MHPMCOUNTER21H:
            CSR_READ(BM_CSR_MHPMCOUNTER21H, value);
            break;
        case BM_CSR_MHPMCOUNTER22H:
            CSR_READ(BM_CSR_MHPMCOUNTER22H, value);
            break;
        case BM_CSR_MHPMCOUNTER23H:
            CSR_READ(BM_CSR_MHPMCOUNTER23H, value);
            break;
        case BM_CSR_MHPMCOUNTER24H:
            CSR_READ(BM_CSR_MHPMCOUNTER24H, value);
            break;
        case BM_CSR_MHPMCOUNTER25H:
            CSR_READ(BM_CSR_MHPMCOUNTER25H, value);
            break;
        case BM_CSR_MHPMCOUNTER26H:
            CSR_READ(BM_CSR_MHPMCOUNTER26H, value);
            break;
        case BM_CSR_MHPMCOUNTER27H:
            CSR_READ(BM_CSR_MHPMCOUNTER27H, value);
            break;
        case BM_CSR_MHPMCOUNTER28H:
            CSR_READ(BM_CSR_MHPMCOUNTER28H, value);
            break;
        case BM_CSR_MHPMCOUNTER29H:
            CSR_READ(BM_CSR_MHPMCOUNTER29H, value);
            break;
        case BM_CSR_MHPMCOUNTER30H:
            CSR_READ(BM_CSR_MHPMCOUNTER30H, value);
            break;
        case BM_CSR_MHPMCOUNTER31H:
            CSR_READ(BM_CSR_MHPMCOUNTER31H, value);
            break;
    #endif
#endif
        case BM_CSR_CYCLE:
            CSR_READ(BM_CSR_CYCLE, value);
            break;
        case BM_CSR_TIME:
            CSR_READ(BM_CSR_TIME, value);
            break;
        case BM_CSR_INSTRET:
            CSR_READ(BM_CSR_INSTRET, value);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3:
            CSR_READ(BM_CSR_HPMCOUNTER3, value);
            break;
        case BM_CSR_HPMCOUNTER4:
            CSR_READ(BM_CSR_HPMCOUNTER4, value);
            break;
        case BM_CSR_HPMCOUNTER5:
            CSR_READ(BM_CSR_HPMCOUNTER5, value);
            break;
        case BM_CSR_HPMCOUNTER6:
            CSR_READ(BM_CSR_HPMCOUNTER6, value);
            break;
        case BM_CSR_HPMCOUNTER7:
            CSR_READ(BM_CSR_HPMCOUNTER7, value);
            break;
        case BM_CSR_HPMCOUNTER8:
            CSR_READ(BM_CSR_HPMCOUNTER8, value);
            break;
        case BM_CSR_HPMCOUNTER9:
            CSR_READ(BM_CSR_HPMCOUNTER9, value);
            break;
        case BM_CSR_HPMCOUNTER10:
            CSR_READ(BM_CSR_HPMCOUNTER10, value);
            break;
        case BM_CSR_HPMCOUNTER11:
            CSR_READ(BM_CSR_HPMCOUNTER11, value);
            break;
        case BM_CSR_HPMCOUNTER12:
            CSR_READ(BM_CSR_HPMCOUNTER12, value);
            break;
        case BM_CSR_HPMCOUNTER13:
            CSR_READ(BM_CSR_HPMCOUNTER13, value);
            break;
        case BM_CSR_HPMCOUNTER14:
            CSR_READ(BM_CSR_HPMCOUNTER14, value);
            break;
        case BM_CSR_HPMCOUNTER15:
            CSR_READ(BM_CSR_HPMCOUNTER15, value);
            break;
        case BM_CSR_HPMCOUNTER16:
            CSR_READ(BM_CSR_HPMCOUNTER16, value);
            break;
        case BM_CSR_HPMCOUNTER17:
            CSR_READ(BM_CSR_HPMCOUNTER17, value);
            break;
        case BM_CSR_HPMCOUNTER18:
            CSR_READ(BM_CSR_HPMCOUNTER18, value);
            break;
        case BM_CSR_HPMCOUNTER19:
            CSR_READ(BM_CSR_HPMCOUNTER19, value);
            break;
        case BM_CSR_HPMCOUNTER20:
            CSR_READ(BM_CSR_HPMCOUNTER20, value);
            break;
        case BM_CSR_HPMCOUNTER21:
            CSR_READ(BM_CSR_HPMCOUNTER21, value);
            break;
        case BM_CSR_HPMCOUNTER22:
            CSR_READ(BM_CSR_HPMCOUNTER22, value);
            break;
        case BM_CSR_HPMCOUNTER23:
            CSR_READ(BM_CSR_HPMCOUNTER23, value);
            break;
        case BM_CSR_HPMCOUNTER24:
            CSR_READ(BM_CSR_HPMCOUNTER24, value);
            break;
        case BM_CSR_HPMCOUNTER25:
            CSR_READ(BM_CSR_HPMCOUNTER25, value);
            break;
        case BM_CSR_HPMCOUNTER26:
            CSR_READ(BM_CSR_HPMCOUNTER26, value);
            break;
        case BM_CSR_HPMCOUNTER27:
            CSR_READ(BM_CSR_HPMCOUNTER27, value);
            break;
        case BM_CSR_HPMCOUNTER28:
            CSR_READ(BM_CSR_HPMCOUNTER28, value);
            break;
        case BM_CSR_HPMCOUNTER29:
            CSR_READ(BM_CSR_HPMCOUNTER29, value);
            break;
        case BM_CSR_HPMCOUNTER30:
            CSR_READ(BM_CSR_HPMCOUNTER30, value);
            break;
        case BM_CSR_HPMCOUNTER31:
            CSR_READ(BM_CSR_HPMCOUNTER31, value);
            break;
#endif
#if __riscv_xlen == 32
        case BM_CSR_CYCLEH:
            CSR_READ(BM_CSR_CYCLEH, value);
            break;
        case BM_CSR_TIMEH:
            CSR_READ(BM_CSR_TIMEH, value);
            break;
        case BM_CSR_INSTRETH:
            CSR_READ(BM_CSR_INSTRETH, value);
            break;
    #ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3H:
            CSR_READ(BM_CSR_HPMCOUNTER3H, value);
            break;
        case BM_CSR_HPMCOUNTER4H:
            CSR_READ(BM_CSR_HPMCOUNTER4H, value);
            break;
        case BM_CSR_HPMCOUNTER5H:
            CSR_READ(BM_CSR_HPMCOUNTER5H, value);
            break;
        case BM_CSR_HPMCOUNTER6H:
            CSR_READ(BM_CSR_HPMCOUNTER6H, value);
            break;
        case BM_CSR_HPMCOUNTER7H:
            CSR_READ(BM_CSR_HPMCOUNTER7H, value);
            break;
        case BM_CSR_HPMCOUNTER8H:
            CSR_READ(BM_CSR_HPMCOUNTER8H, value);
            break;
        case BM_CSR_HPMCOUNTER9H:
            CSR_READ(BM_CSR_HPMCOUNTER9H, value);
            break;
        case BM_CSR_HPMCOUNTER10H:
            CSR_READ(BM_CSR_HPMCOUNTER10H, value);
            break;
        case BM_CSR_HPMCOUNTER11H:
            CSR_READ(BM_CSR_HPMCOUNTER11H, value);
            break;
        case BM_CSR_HPMCOUNTER12H:
            CSR_READ(BM_CSR_HPMCOUNTER12H, value);
            break;
        case BM_CSR_HPMCOUNTER13H:
            CSR_READ(BM_CSR_HPMCOUNTER13H, value);
            break;
        case BM_CSR_HPMCOUNTER14H:
            CSR_READ(BM_CSR_HPMCOUNTER14H, value);
            break;
        case BM_CSR_HPMCOUNTER15H:
            CSR_READ(BM_CSR_HPMCOUNTER15H, value);
            break;
        case BM_CSR_HPMCOUNTER16H:
            CSR_READ(BM_CSR_HPMCOUNTER16H, value);
            break;
        case BM_CSR_HPMCOUNTER17H:
            CSR_READ(BM_CSR_HPMCOUNTER17H, value);
            break;
        case BM_CSR_HPMCOUNTER18H:
            CSR_READ(BM_CSR_HPMCOUNTER18H, value);
            break;
        case BM_CSR_HPMCOUNTER19H:
            CSR_READ(BM_CSR_HPMCOUNTER19H, value);
            break;
        case BM_CSR_HPMCOUNTER20H:
            CSR_READ(BM_CSR_HPMCOUNTER20H, value);
            break;
        case BM_CSR_HPMCOUNTER21H:
            CSR_READ(BM_CSR_HPMCOUNTER21H, value);
            break;
        case BM_CSR_HPMCOUNTER22H:
            CSR_READ(BM_CSR_HPMCOUNTER22H, value);
            break;
        case BM_CSR_HPMCOUNTER23H:
            CSR_READ(BM_CSR_HPMCOUNTER23H, value);
            break;
        case BM_CSR_HPMCOUNTER24H:
            CSR_READ(BM_CSR_HPMCOUNTER24H, value);
            break;
        case BM_CSR_HPMCOUNTER25H:
            CSR_READ(BM_CSR_HPMCOUNTER25H, value);
            break;
        case BM_CSR_HPMCOUNTER26H:
            CSR_READ(BM_CSR_HPMCOUNTER26H, value);
            break;
        case BM_CSR_HPMCOUNTER27H:
            CSR_READ(BM_CSR_HPMCOUNTER27H, value);
            break;
        case BM_CSR_HPMCOUNTER28H:
            CSR_READ(BM_CSR_HPMCOUNTER28H, value);
            break;
        case BM_CSR_HPMCOUNTER29H:
            CSR_READ(BM_CSR_HPMCOUNTER29H, value);
            break;
        case BM_CSR_HPMCOUNTER30H:
            CSR_READ(BM_CSR_HPMCOUNTER30H, value);
            break;
        case BM_CSR_HPMCOUNTER31H:
            CSR_READ(BM_CSR_HPMCOUNTER31H, value);
            break;
    #endif
#endif
        case BM_CSR_MVENDORID:
            CSR_READ(BM_CSR_MVENDORID, value);
            break;
        case BM_CSR_MARCHID:
            CSR_READ(BM_CSR_MARCHID, value);
            break;
        case BM_CSR_MIMPID:
            CSR_READ(BM_CSR_MIMPID, value);
            break;
        case BM_CSR_MHARTID:
            CSR_READ(BM_CSR_MHARTID, value);
            break;
        default:
#ifdef TARGET_HAS_CUSTOM_CSR
            value = target_csr_read(csr);
#else
            bm_error("Unsupported CSR.");
#endif
            break;
    }
    return value;
}

void bm_csr_write(const int csr, const xlen_t value)
{
    switch (csr)
    {
#ifdef TARGET_EXT_N
        case BM_CSR_USTATUS:
            CSR_WRITE(BM_CSR_USTATUS, value);
            break;
#endif
#ifdef __riscv_flen
        case BM_CSR_FFLAGS:
            CSR_WRITE(BM_CSR_FFLAGS, value);
            break;
        case BM_CSR_FRM:
            CSR_WRITE(BM_CSR_FRM, value);
            break;
        case BM_CSR_FCSR:
            CSR_WRITE(BM_CSR_FCSR, value);
            break;
#endif
#ifdef TARGET_EXT_N
        case BM_CSR_UIE:
            CSR_WRITE(BM_CSR_UIE, value);
            break;
        case BM_CSR_UTVEC:
            CSR_WRITE(BM_CSR_UTVEC, value);
            break;
        case BM_CSR_USCRATCH:
            CSR_WRITE(BM_CSR_USCRATCH, value);
            break;
        case BM_CSR_UEPC:
            CSR_WRITE(BM_CSR_UEPC, value);
            break;
        case BM_CSR_UCAUSE:
            CSR_WRITE(BM_CSR_UCAUSE, value);
            break;
        case BM_CSR_UTVAL:
            CSR_WRITE(BM_CSR_UTVAL, value);
            break;
        case BM_CSR_UIP:
            CSR_WRITE(BM_CSR_UIP, value);
            break;
#endif
        case BM_CSR_SEED:
            CSR_WRITE(BM_CSR_SEED, value);
            break;
#ifdef TARGET_EXT_S
        case BM_CSR_SSTATUS:
            CSR_WRITE(BM_CSR_SSTATUS, value);
            break;
    #ifdef TARGET_EXT_N
        case BM_CSR_SEDELEG:
            CSR_WRITE(BM_CSR_SEDELEG, value);
            break;
        case BM_CSR_SIDELEG:
            CSR_WRITE(BM_CSR_SIDELEG, value);
            break;
    #endif
        case BM_CSR_SIE:
            CSR_WRITE(BM_CSR_SIE, value);
            break;
        case BM_CSR_STVEC:
            CSR_WRITE(BM_CSR_STVEC, value);
            break;
        case BM_CSR_SCOUNTEREN:
            CSR_WRITE(BM_CSR_SCOUNTEREN, value);
            break;
        case BM_CSR_SSCRATCH:
            CSR_WRITE(BM_CSR_SSCRATCH, value);
            break;
        case BM_CSR_SEPC:
            CSR_WRITE(BM_CSR_SEPC, value);
            break;
        case BM_CSR_SCAUSE:
            CSR_WRITE(BM_CSR_SCAUSE, value);
            break;
        case BM_CSR_STVAL:
            CSR_WRITE(BM_CSR_STVAL, value);
            break;
        case BM_CSR_SIP:
            CSR_WRITE(BM_CSR_SIP, value);
            break;
        case BM_CSR_SATP:
            CSR_WRITE(BM_CSR_SATP, value);
            break;
    #ifdef TARGET_HAS_CLIC
        case BM_CSR_STVT:
            CSR_WRITE(BM_CSR_STVT, value);
            break;
        case BM_CSR_SNXTI:
            CSR_WRITE(BM_CSR_SNXTI, value);
            break;
        case BM_CSR_SINTTHRESH:
            CSR_WRITE(BM_CSR_SINTTHRESH, value);
            break;
        case BM_CSR_SSCRATCHCSW:
            CSR_WRITE(BM_CSR_SSCRATCHCSW, value);
            break;
        case BM_CSR_SSCRATCHCSWL:
            CSR_WRITE(BM_CSR_SSCRATCHCSWL, value);
            break;
        case BM_CSR_SINTSTATUS:
            CSR_WRITE(BM_CSR_SINTSTATUS, value);
            break;
    #endif
        case BM_CSR_SENVCFG:
            CSR_WRITE(BM_CSR_SENVCFG, value);
            break;
#endif
        case BM_CSR_MSTATUS:
            CSR_WRITE(BM_CSR_MSTATUS, value);
            break;
        case BM_CSR_MISA:
            CSR_WRITE(BM_CSR_MISA, value);
            break;
#if defined(TARGET_EXT_S) || defined(TARGET_EXT_N)
        case BM_CSR_MEDELEG:
            CSR_WRITE(BM_CSR_MEDELEG, value);
            break;
        case BM_CSR_MIDELEG:
            CSR_WRITE(BM_CSR_MIDELEG, value);
            break;
#endif
        case BM_CSR_MIE:
            CSR_WRITE(BM_CSR_MIE, value);
            break;
        case BM_CSR_MTVEC:
            CSR_WRITE(BM_CSR_MTVEC, value);
            break;
#ifdef TARGET_HAS_CLIC
        case BM_CSR_MTVT:
            CSR_WRITE(BM_CSR_MTVT, value);
            break;
        case BM_CSR_MNXTI:
            CSR_WRITE(BM_CSR_MNXTI, value);
            break;
        case BM_CSR_MINTTHRESH:
            CSR_WRITE(BM_CSR_MINTTHRESH, value);
            break;
        case BM_CSR_MSCRATCHCSW:
            CSR_WRITE(BM_CSR_MSCRATCHCSW, value);
            break;
        case BM_CSR_MSCRATCHCSWL:
            CSR_WRITE(BM_CSR_MSCRATCHCSWL, value);
            break;
        case BM_CSR_MINTSTATUS:
            CSR_WRITE(BM_CSR_MINTSTATUS, value);
            break;
#endif
        case BM_CSR_MENVCFG:
            CSR_WRITE(BM_CSR_MENVCFG, value);
            break;
#if __riscv_xlen == 32
        case BM_CSR_MENVCFGH:
            CSR_WRITE(BM_CSR_MENVCFGH, value);
            break;
#endif
        case BM_CSR_MCOUNTEREN:
            CSR_WRITE(BM_CSR_MCOUNTEREN, value);
            break;
        case BM_CSR_MCOUNTINHIBIT:
            CSR_WRITE(BM_CSR_MCOUNTINHIBIT, value);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMEVENT3:
            CSR_WRITE(BM_CSR_MHPMEVENT3, value);
            break;
        case BM_CSR_MHPMEVENT4:
            CSR_WRITE(BM_CSR_MHPMEVENT4, value);
            break;
        case BM_CSR_MHPMEVENT5:
            CSR_WRITE(BM_CSR_MHPMEVENT5, value);
            break;
        case BM_CSR_MHPMEVENT6:
            CSR_WRITE(BM_CSR_MHPMEVENT6, value);
            break;
        case BM_CSR_MHPMEVENT7:
            CSR_WRITE(BM_CSR_MHPMEVENT7, value);
            break;
        case BM_CSR_MHPMEVENT8:
            CSR_WRITE(BM_CSR_MHPMEVENT8, value);
            break;
        case BM_CSR_MHPMEVENT9:
            CSR_WRITE(BM_CSR_MHPMEVENT9, value);
            break;
        case BM_CSR_MHPMEVENT10:
            CSR_WRITE(BM_CSR_MHPMEVENT10, value);
            break;
        case BM_CSR_MHPMEVENT11:
            CSR_WRITE(BM_CSR_MHPMEVENT11, value);
            break;
        case BM_CSR_MHPMEVENT12:
            CSR_WRITE(BM_CSR_MHPMEVENT12, value);
            break;
        case BM_CSR_MHPMEVENT13:
            CSR_WRITE(BM_CSR_MHPMEVENT13, value);
            break;
        case BM_CSR_MHPMEVENT14:
            CSR_WRITE(BM_CSR_MHPMEVENT14, value);
            break;
        case BM_CSR_MHPMEVENT15:
            CSR_WRITE(BM_CSR_MHPMEVENT15, value);
            break;
        case BM_CSR_MHPMEVENT16:
            CSR_WRITE(BM_CSR_MHPMEVENT16, value);
            break;
        case BM_CSR_MHPMEVENT17:
            CSR_WRITE(BM_CSR_MHPMEVENT17, value);
            break;
        case BM_CSR_MHPMEVENT18:
            CSR_WRITE(BM_CSR_MHPMEVENT18, value);
            break;
        case BM_CSR_MHPMEVENT19:
            CSR_WRITE(BM_CSR_MHPMEVENT19, value);
            break;
        case BM_CSR_MHPMEVENT20:
            CSR_WRITE(BM_CSR_MHPMEVENT20, value);
            break;
        case BM_CSR_MHPMEVENT21:
            CSR_WRITE(BM_CSR_MHPMEVENT21, value);
            break;
        case BM_CSR_MHPMEVENT22:
            CSR_WRITE(BM_CSR_MHPMEVENT22, value);
            break;
        case BM_CSR_MHPMEVENT23:
            CSR_WRITE(BM_CSR_MHPMEVENT23, value);
            break;
        case BM_CSR_MHPMEVENT24:
            CSR_WRITE(BM_CSR_MHPMEVENT24, value);
            break;
        case BM_CSR_MHPMEVENT25:
            CSR_WRITE(BM_CSR_MHPMEVENT25, value);
            break;
        case BM_CSR_MHPMEVENT26:
            CSR_WRITE(BM_CSR_MHPMEVENT26, value);
            break;
        case BM_CSR_MHPMEVENT27:
            CSR_WRITE(BM_CSR_MHPMEVENT27, value);
            break;
        case BM_CSR_MHPMEVENT28:
            CSR_WRITE(BM_CSR_MHPMEVENT28, value);
            break;
        case BM_CSR_MHPMEVENT29:
            CSR_WRITE(BM_CSR_MHPMEVENT29, value);
            break;
        case BM_CSR_MHPMEVENT30:
            CSR_WRITE(BM_CSR_MHPMEVENT30, value);
            break;
        case BM_CSR_MHPMEVENT31:
            CSR_WRITE(BM_CSR_MHPMEVENT31, value);
            break;
#endif
        case BM_CSR_MSCRATCH:
            CSR_WRITE(BM_CSR_MSCRATCH, value);
            break;
        case BM_CSR_MEPC:
            CSR_WRITE(BM_CSR_MEPC, value);
            break;
        case BM_CSR_MCAUSE:
            CSR_WRITE(BM_CSR_MCAUSE, value);
            break;
        case BM_CSR_MTVAL:
            CSR_WRITE(BM_CSR_MTVAL, value);
            break;
        case BM_CSR_MIP:
            CSR_WRITE(BM_CSR_MIP, value);
            break;
#ifdef TARGET_HAS_PMP
        case BM_CSR_PMPCFG0:
            CSR_WRITE(BM_CSR_PMPCFG0, value);
            break;
    #if TARGET_HAS_PMP_CFG_REG1
        case BM_CSR_PMPCFG1:
            CSR_WRITE(BM_CSR_PMPCFG1, value);
            break;
    #endif
    #if TARGET_HAS_PMP_CFG_REG2
        case BM_CSR_PMPCFG2:
            CSR_WRITE(BM_CSR_PMPCFG2, value);
            break;
    #endif
    #if TARGET_HAS_PMP_CFG_REG3
        case BM_CSR_PMPCFG3:
            CSR_WRITE(BM_CSR_PMPCFG3, value);
            break;
    #endif
        case BM_CSR_PMPADDR0:
            CSR_WRITE(BM_CSR_PMPADDR0, value);
            break;
        case BM_CSR_PMPADDR1:
            CSR_WRITE(BM_CSR_PMPADDR1, value);
            break;
        case BM_CSR_PMPADDR2:
            CSR_WRITE(BM_CSR_PMPADDR2, value);
            break;
        case BM_CSR_PMPADDR3:
            CSR_WRITE(BM_CSR_PMPADDR3, value);
            break;
    #if TARGET_PMP_NUM_REGIONS > 4
        case BM_CSR_PMPADDR4:
            CSR_WRITE(BM_CSR_PMPADDR4, value);
            break;
        case BM_CSR_PMPADDR5:
            CSR_WRITE(BM_CSR_PMPADDR5, value);
            break;
        case BM_CSR_PMPADDR6:
            CSR_WRITE(BM_CSR_PMPADDR6, value);
            break;
        case BM_CSR_PMPADDR7:
            CSR_WRITE(BM_CSR_PMPADDR7, value);
            break;
    #endif
    #if TARGET_PMP_NUM_REGIONS > 8
        case BM_CSR_PMPADDR8:
            CSR_WRITE(BM_CSR_PMPADDR8, value);
            break;
        case BM_CSR_PMPADDR9:
            CSR_WRITE(BM_CSR_PMPADDR9, value);
            break;
        case BM_CSR_PMPADDR10:
            CSR_WRITE(BM_CSR_PMPADDR10, value);
            break;
        case BM_CSR_PMPADDR11:
            CSR_WRITE(BM_CSR_PMPADDR11, value);
            break;
    #endif
    #if TARGET_PMP_NUM_REGIONS > 12
        case BM_CSR_PMPADDR12:
            CSR_WRITE(BM_CSR_PMPADDR12, value);
            break;
        case BM_CSR_PMPADDR13:
            CSR_WRITE(BM_CSR_PMPADDR13, value);
            break;
        case BM_CSR_PMPADDR14:
            CSR_WRITE(BM_CSR_PMPADDR14, value);
            break;
        case BM_CSR_PMPADDR15:
            CSR_WRITE(BM_CSR_PMPADDR15, value);
            break;
    #endif
#endif
        case BM_CSR_MSECCFG:
            CSR_WRITE(BM_CSR_MSECCFG, value);
            break;
#if __riscv_xlen == 32
        case BM_CSR_MSECCFGH:
            CSR_WRITE(BM_CSR_MSECCFGH, value);
            break;
#endif
        case BM_CSR_TSELECT:
            CSR_WRITE(BM_CSR_TSELECT, value);
            break;
        case BM_CSR_TDATA1:
            CSR_WRITE(BM_CSR_TDATA1, value);
            break;
        case BM_CSR_TDATA2:
            CSR_WRITE(BM_CSR_TDATA2, value);
            break;
        case BM_CSR_TDATA3:
            CSR_WRITE(BM_CSR_TDATA3, value);
            break;
        case BM_CSR_TINFO:
            CSR_WRITE(BM_CSR_TINFO, value);
            break;
        case BM_CSR_TCONTROL:
            CSR_WRITE(BM_CSR_TCONTROL, value);
            break;
        case BM_CSR_MCONTEXT:
            CSR_WRITE(BM_CSR_MCONTEXT, value);
            break;
#ifdef TARGET_HAS_PIC
        case BM_CSR_MPICFLAG:
            CSR_WRITE(BM_CSR_MPICFLAG, value);
            break;
        case BM_CSR_MPICMASK:
            CSR_WRITE(BM_CSR_MPICMASK, value);
            break;
    #if TARGET_PIC_NUM_INTERRUPTS > 32
        case BM_CSR_MPICFLAG1:
            CSR_WRITE(BM_CSR_MPICFLAG1, value);
            break;
        case BM_CSR_MPICMASK1:
            CSR_WRITE(BM_CSR_MPICMASK1, value);
            break;
    #endif
    #if TARGET_PIC_NUM_INTERRUPTS > 64
        case BM_CSR_MPICFLAG2:
            CSR_WRITE(BM_CSR_MPICFLAG2, value);
            break;
        case BM_CSR_MPICMASK2:
            CSR_WRITE(BM_CSR_MPICMASK2, value);
            break;
    #endif
    #if TARGET_PIC_NUM_INTERRUPTS > 96
        case BM_CSR_MPICFLAG3:
            CSR_WRITE(BM_CSR_MPICFLAG3, value);
            break;
        case BM_CSR_MPICMASK3:
            CSR_WRITE(BM_CSR_MPICMASK3, value);
            break;
    #endif
#endif
        case BM_CSR_MCYCLE:
            CSR_WRITE(BM_CSR_MCYCLE, value);
            break;
        case BM_CSR_MINSTRET:
            CSR_WRITE(BM_CSR_MINSTRET, value);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMCOUNTER3:
            CSR_WRITE(BM_CSR_MHPMCOUNTER3, value);
            break;
        case BM_CSR_MHPMCOUNTER4:
            CSR_WRITE(BM_CSR_MHPMCOUNTER4, value);
            break;
        case BM_CSR_MHPMCOUNTER5:
            CSR_WRITE(BM_CSR_MHPMCOUNTER5, value);
            break;
        case BM_CSR_MHPMCOUNTER6:
            CSR_WRITE(BM_CSR_MHPMCOUNTER6, value);
            break;
        case BM_CSR_MHPMCOUNTER7:
            CSR_WRITE(BM_CSR_MHPMCOUNTER7, value);
            break;
        case BM_CSR_MHPMCOUNTER8:
            CSR_WRITE(BM_CSR_MHPMCOUNTER8, value);
            break;
        case BM_CSR_MHPMCOUNTER9:
            CSR_WRITE(BM_CSR_MHPMCOUNTER9, value);
            break;
        case BM_CSR_MHPMCOUNTER10:
            CSR_WRITE(BM_CSR_MHPMCOUNTER10, value);
            break;
        case BM_CSR_MHPMCOUNTER11:
            CSR_WRITE(BM_CSR_MHPMCOUNTER11, value);
            break;
        case BM_CSR_MHPMCOUNTER12:
            CSR_WRITE(BM_CSR_MHPMCOUNTER12, value);
            break;
        case BM_CSR_MHPMCOUNTER13:
            CSR_WRITE(BM_CSR_MHPMCOUNTER13, value);
            break;
        case BM_CSR_MHPMCOUNTER14:
            CSR_WRITE(BM_CSR_MHPMCOUNTER14, value);
            break;
        case BM_CSR_MHPMCOUNTER15:
            CSR_WRITE(BM_CSR_MHPMCOUNTER15, value);
            break;
        case BM_CSR_MHPMCOUNTER16:
            CSR_WRITE(BM_CSR_MHPMCOUNTER16, value);
            break;
        case BM_CSR_MHPMCOUNTER17:
            CSR_WRITE(BM_CSR_MHPMCOUNTER17, value);
            break;
        case BM_CSR_MHPMCOUNTER18:
            CSR_WRITE(BM_CSR_MHPMCOUNTER18, value);
            break;
        case BM_CSR_MHPMCOUNTER19:
            CSR_WRITE(BM_CSR_MHPMCOUNTER19, value);
            break;
        case BM_CSR_MHPMCOUNTER20:
            CSR_WRITE(BM_CSR_MHPMCOUNTER20, value);
            break;
        case BM_CSR_MHPMCOUNTER21:
            CSR_WRITE(BM_CSR_MHPMCOUNTER21, value);
            break;
        case BM_CSR_MHPMCOUNTER22:
            CSR_WRITE(BM_CSR_MHPMCOUNTER22, value);
            break;
        case BM_CSR_MHPMCOUNTER23:
            CSR_WRITE(BM_CSR_MHPMCOUNTER23, value);
            break;
        case BM_CSR_MHPMCOUNTER24:
            CSR_WRITE(BM_CSR_MHPMCOUNTER24, value);
            break;
        case BM_CSR_MHPMCOUNTER25:
            CSR_WRITE(BM_CSR_MHPMCOUNTER25, value);
            break;
        case BM_CSR_MHPMCOUNTER26:
            CSR_WRITE(BM_CSR_MHPMCOUNTER26, value);
            break;
        case BM_CSR_MHPMCOUNTER27:
            CSR_WRITE(BM_CSR_MHPMCOUNTER27, value);
            break;
        case BM_CSR_MHPMCOUNTER28:
            CSR_WRITE(BM_CSR_MHPMCOUNTER28, value);
            break;
        case BM_CSR_MHPMCOUNTER29:
            CSR_WRITE(BM_CSR_MHPMCOUNTER29, value);
            break;
        case BM_CSR_MHPMCOUNTER30:
            CSR_WRITE(BM_CSR_MHPMCOUNTER30, value);
            break;
        case BM_CSR_MHPMCOUNTER31:
            CSR_WRITE(BM_CSR_MHPMCOUNTER31, value);
            break;
#endif
#if __riscv_xlen == 32
        case BM_CSR_MCYCLEH:
            CSR_WRITE(BM_CSR_MCYCLEH, value);
            break;
        case BM_CSR_MINSTRETH:
            CSR_WRITE(BM_CSR_MINSTRETH, value);
            break;
    #ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMCOUNTER3H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER3H, value);
            break;
        case BM_CSR_MHPMCOUNTER4H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER4H, value);
            break;
        case BM_CSR_MHPMCOUNTER5H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER5H, value);
            break;
        case BM_CSR_MHPMCOUNTER6H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER6H, value);
            break;
        case BM_CSR_MHPMCOUNTER7H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER7H, value);
            break;
        case BM_CSR_MHPMCOUNTER8H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER8H, value);
            break;
        case BM_CSR_MHPMCOUNTER9H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER9H, value);
            break;
        case BM_CSR_MHPMCOUNTER10H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER10H, value);
            break;
        case BM_CSR_MHPMCOUNTER11H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER11H, value);
            break;
        case BM_CSR_MHPMCOUNTER12H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER12H, value);
            break;
        case BM_CSR_MHPMCOUNTER13H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER13H, value);
            break;
        case BM_CSR_MHPMCOUNTER14H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER14H, value);
            break;
        case BM_CSR_MHPMCOUNTER15H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER15H, value);
            break;
        case BM_CSR_MHPMCOUNTER16H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER16H, value);
            break;
        case BM_CSR_MHPMCOUNTER17H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER17H, value);
            break;
        case BM_CSR_MHPMCOUNTER18H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER18H, value);
            break;
        case BM_CSR_MHPMCOUNTER19H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER19H, value);
            break;
        case BM_CSR_MHPMCOUNTER20H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER20H, value);
            break;
        case BM_CSR_MHPMCOUNTER21H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER21H, value);
            break;
        case BM_CSR_MHPMCOUNTER22H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER22H, value);
            break;
        case BM_CSR_MHPMCOUNTER23H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER23H, value);
            break;
        case BM_CSR_MHPMCOUNTER24H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER24H, value);
            break;
        case BM_CSR_MHPMCOUNTER25H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER25H, value);
            break;
        case BM_CSR_MHPMCOUNTER26H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER26H, value);
            break;
        case BM_CSR_MHPMCOUNTER27H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER27H, value);
            break;
        case BM_CSR_MHPMCOUNTER28H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER28H, value);
            break;
        case BM_CSR_MHPMCOUNTER29H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER29H, value);
            break;
        case BM_CSR_MHPMCOUNTER30H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER30H, value);
            break;
        case BM_CSR_MHPMCOUNTER31H:
            CSR_WRITE(BM_CSR_MHPMCOUNTER31H, value);
            break;
    #endif
#endif
        case BM_CSR_CYCLE:
            CSR_WRITE(BM_CSR_CYCLE, value);
            break;
        case BM_CSR_TIME:
            CSR_WRITE(BM_CSR_TIME, value);
            break;
        case BM_CSR_INSTRET:
            CSR_WRITE(BM_CSR_INSTRET, value);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3:
            CSR_WRITE(BM_CSR_HPMCOUNTER3, value);
            break;
        case BM_CSR_HPMCOUNTER4:
            CSR_WRITE(BM_CSR_HPMCOUNTER4, value);
            break;
        case BM_CSR_HPMCOUNTER5:
            CSR_WRITE(BM_CSR_HPMCOUNTER5, value);
            break;
        case BM_CSR_HPMCOUNTER6:
            CSR_WRITE(BM_CSR_HPMCOUNTER6, value);
            break;
        case BM_CSR_HPMCOUNTER7:
            CSR_WRITE(BM_CSR_HPMCOUNTER7, value);
            break;
        case BM_CSR_HPMCOUNTER8:
            CSR_WRITE(BM_CSR_HPMCOUNTER8, value);
            break;
        case BM_CSR_HPMCOUNTER9:
            CSR_WRITE(BM_CSR_HPMCOUNTER9, value);
            break;
        case BM_CSR_HPMCOUNTER10:
            CSR_WRITE(BM_CSR_HPMCOUNTER10, value);
            break;
        case BM_CSR_HPMCOUNTER11:
            CSR_WRITE(BM_CSR_HPMCOUNTER11, value);
            break;
        case BM_CSR_HPMCOUNTER12:
            CSR_WRITE(BM_CSR_HPMCOUNTER12, value);
            break;
        case BM_CSR_HPMCOUNTER13:
            CSR_WRITE(BM_CSR_HPMCOUNTER13, value);
            break;
        case BM_CSR_HPMCOUNTER14:
            CSR_WRITE(BM_CSR_HPMCOUNTER14, value);
            break;
        case BM_CSR_HPMCOUNTER15:
            CSR_WRITE(BM_CSR_HPMCOUNTER15, value);
            break;
        case BM_CSR_HPMCOUNTER16:
            CSR_WRITE(BM_CSR_HPMCOUNTER16, value);
            break;
        case BM_CSR_HPMCOUNTER17:
            CSR_WRITE(BM_CSR_HPMCOUNTER17, value);
            break;
        case BM_CSR_HPMCOUNTER18:
            CSR_WRITE(BM_CSR_HPMCOUNTER18, value);
            break;
        case BM_CSR_HPMCOUNTER19:
            CSR_WRITE(BM_CSR_HPMCOUNTER19, value);
            break;
        case BM_CSR_HPMCOUNTER20:
            CSR_WRITE(BM_CSR_HPMCOUNTER20, value);
            break;
        case BM_CSR_HPMCOUNTER21:
            CSR_WRITE(BM_CSR_HPMCOUNTER21, value);
            break;
        case BM_CSR_HPMCOUNTER22:
            CSR_WRITE(BM_CSR_HPMCOUNTER22, value);
            break;
        case BM_CSR_HPMCOUNTER23:
            CSR_WRITE(BM_CSR_HPMCOUNTER23, value);
            break;
        case BM_CSR_HPMCOUNTER24:
            CSR_WRITE(BM_CSR_HPMCOUNTER24, value);
            break;
        case BM_CSR_HPMCOUNTER25:
            CSR_WRITE(BM_CSR_HPMCOUNTER25, value);
            break;
        case BM_CSR_HPMCOUNTER26:
            CSR_WRITE(BM_CSR_HPMCOUNTER26, value);
            break;
        case BM_CSR_HPMCOUNTER27:
            CSR_WRITE(BM_CSR_HPMCOUNTER27, value);
            break;
        case BM_CSR_HPMCOUNTER28:
            CSR_WRITE(BM_CSR_HPMCOUNTER28, value);
            break;
        case BM_CSR_HPMCOUNTER29:
            CSR_WRITE(BM_CSR_HPMCOUNTER29, value);
            break;
        case BM_CSR_HPMCOUNTER30:
            CSR_WRITE(BM_CSR_HPMCOUNTER30, value);
            break;
        case BM_CSR_HPMCOUNTER31:
            CSR_WRITE(BM_CSR_HPMCOUNTER31, value);
            break;
#endif
#if __riscv_xlen == 32
        case BM_CSR_CYCLEH:
            CSR_WRITE(BM_CSR_CYCLEH, value);
            break;
        case BM_CSR_TIMEH:
            CSR_WRITE(BM_CSR_TIMEH, value);
            break;
        case BM_CSR_INSTRETH:
            CSR_WRITE(BM_CSR_INSTRETH, value);
            break;
    #ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3H:
            CSR_WRITE(BM_CSR_HPMCOUNTER3H, value);
            break;
        case BM_CSR_HPMCOUNTER4H:
            CSR_WRITE(BM_CSR_HPMCOUNTER4H, value);
            break;
        case BM_CSR_HPMCOUNTER5H:
            CSR_WRITE(BM_CSR_HPMCOUNTER5H, value);
            break;
        case BM_CSR_HPMCOUNTER6H:
            CSR_WRITE(BM_CSR_HPMCOUNTER6H, value);
            break;
        case BM_CSR_HPMCOUNTER7H:
            CSR_WRITE(BM_CSR_HPMCOUNTER7H, value);
            break;
        case BM_CSR_HPMCOUNTER8H:
            CSR_WRITE(BM_CSR_HPMCOUNTER8H, value);
            break;
        case BM_CSR_HPMCOUNTER9H:
            CSR_WRITE(BM_CSR_HPMCOUNTER9H, value);
            break;
        case BM_CSR_HPMCOUNTER10H:
            CSR_WRITE(BM_CSR_HPMCOUNTER10H, value);
            break;
        case BM_CSR_HPMCOUNTER11H:
            CSR_WRITE(BM_CSR_HPMCOUNTER11H, value);
            break;
        case BM_CSR_HPMCOUNTER12H:
            CSR_WRITE(BM_CSR_HPMCOUNTER12H, value);
            break;
        case BM_CSR_HPMCOUNTER13H:
            CSR_WRITE(BM_CSR_HPMCOUNTER13H, value);
            break;
        case BM_CSR_HPMCOUNTER14H:
            CSR_WRITE(BM_CSR_HPMCOUNTER14H, value);
            break;
        case BM_CSR_HPMCOUNTER15H:
            CSR_WRITE(BM_CSR_HPMCOUNTER15H, value);
            break;
        case BM_CSR_HPMCOUNTER16H:
            CSR_WRITE(BM_CSR_HPMCOUNTER16H, value);
            break;
        case BM_CSR_HPMCOUNTER17H:
            CSR_WRITE(BM_CSR_HPMCOUNTER17H, value);
            break;
        case BM_CSR_HPMCOUNTER18H:
            CSR_WRITE(BM_CSR_HPMCOUNTER18H, value);
            break;
        case BM_CSR_HPMCOUNTER19H:
            CSR_WRITE(BM_CSR_HPMCOUNTER19H, value);
            break;
        case BM_CSR_HPMCOUNTER20H:
            CSR_WRITE(BM_CSR_HPMCOUNTER20H, value);
            break;
        case BM_CSR_HPMCOUNTER21H:
            CSR_WRITE(BM_CSR_HPMCOUNTER21H, value);
            break;
        case BM_CSR_HPMCOUNTER22H:
            CSR_WRITE(BM_CSR_HPMCOUNTER22H, value);
            break;
        case BM_CSR_HPMCOUNTER23H:
            CSR_WRITE(BM_CSR_HPMCOUNTER23H, value);
            break;
        case BM_CSR_HPMCOUNTER24H:
            CSR_WRITE(BM_CSR_HPMCOUNTER24H, value);
            break;
        case BM_CSR_HPMCOUNTER25H:
            CSR_WRITE(BM_CSR_HPMCOUNTER25H, value);
            break;
        case BM_CSR_HPMCOUNTER26H:
            CSR_WRITE(BM_CSR_HPMCOUNTER26H, value);
            break;
        case BM_CSR_HPMCOUNTER27H:
            CSR_WRITE(BM_CSR_HPMCOUNTER27H, value);
            break;
        case BM_CSR_HPMCOUNTER28H:
            CSR_WRITE(BM_CSR_HPMCOUNTER28H, value);
            break;
        case BM_CSR_HPMCOUNTER29H:
            CSR_WRITE(BM_CSR_HPMCOUNTER29H, value);
            break;
        case BM_CSR_HPMCOUNTER30H:
            CSR_WRITE(BM_CSR_HPMCOUNTER30H, value);
            break;
        case BM_CSR_HPMCOUNTER31H:
            CSR_WRITE(BM_CSR_HPMCOUNTER31H, value);
            break;
    #endif
#endif
        case BM_CSR_MVENDORID:
            CSR_WRITE(BM_CSR_MVENDORID, value);
            break;
        case BM_CSR_MARCHID:
            CSR_WRITE(BM_CSR_MARCHID, value);
            break;
        case BM_CSR_MIMPID:
            CSR_WRITE(BM_CSR_MIMPID, value);
            break;
        case BM_CSR_MHARTID:
            CSR_WRITE(BM_CSR_MHARTID, value);
            break;
        default:
#ifdef TARGET_HAS_CUSTOM_CSR
            target_csr_write(csr, value);
#else
            bm_error("Unsupported CSR.");
#endif
            break;
    }
}

void bm_csr_set_mask(const int csr, const xlen_t mask)
{
    switch (csr)
    {
#ifdef TARGET_EXT_N
        case BM_CSR_USTATUS:
            CSR_SET(BM_CSR_USTATUS, mask);
            break;
#endif
#ifdef __riscv_flen
        case BM_CSR_FFLAGS:
            CSR_SET(BM_CSR_FFLAGS, mask);
            break;
        case BM_CSR_FRM:
            CSR_SET(BM_CSR_FRM, mask);
            break;
        case BM_CSR_FCSR:
            CSR_SET(BM_CSR_FCSR, mask);
            break;
#endif
#ifdef TARGET_EXT_N
        case BM_CSR_UIE:
            CSR_SET(BM_CSR_UIE, mask);
            break;
        case BM_CSR_UTVEC:
            CSR_SET(BM_CSR_UTVEC, mask);
            break;
        case BM_CSR_USCRATCH:
            CSR_SET(BM_CSR_USCRATCH, mask);
            break;
        case BM_CSR_UEPC:
            CSR_SET(BM_CSR_UEPC, mask);
            break;
        case BM_CSR_UCAUSE:
            CSR_SET(BM_CSR_UCAUSE, mask);
            break;
        case BM_CSR_UTVAL:
            CSR_SET(BM_CSR_UTVAL, mask);
            break;
        case BM_CSR_UIP:
            CSR_SET(BM_CSR_UIP, mask);
            break;
#endif
        case BM_CSR_SEED:
            CSR_SET(BM_CSR_SEED, mask);
            break;
#ifdef TARGET_EXT_S
        case BM_CSR_SSTATUS:
            CSR_SET(BM_CSR_SSTATUS, mask);
            break;
    #ifdef TARGET_EXT_N
        case BM_CSR_SEDELEG:
            CSR_SET(BM_CSR_SEDELEG, mask);
            break;
        case BM_CSR_SIDELEG:
            CSR_SET(BM_CSR_SIDELEG, mask);
            break;
    #endif
        case BM_CSR_SIE:
            CSR_SET(BM_CSR_SIE, mask);
            break;
        case BM_CSR_STVEC:
            CSR_SET(BM_CSR_STVEC, mask);
            break;
        case BM_CSR_SCOUNTEREN:
            CSR_SET(BM_CSR_SCOUNTEREN, mask);
            break;
        case BM_CSR_SSCRATCH:
            CSR_SET(BM_CSR_SSCRATCH, mask);
            break;
        case BM_CSR_SEPC:
            CSR_SET(BM_CSR_SEPC, mask);
            break;
        case BM_CSR_SCAUSE:
            CSR_SET(BM_CSR_SCAUSE, mask);
            break;
        case BM_CSR_STVAL:
            CSR_SET(BM_CSR_STVAL, mask);
            break;
        case BM_CSR_SIP:
            CSR_SET(BM_CSR_SIP, mask);
            break;
        case BM_CSR_SATP:
            CSR_SET(BM_CSR_SATP, mask);
            break;
    #ifdef TARGET_HAS_CLIC
        case BM_CSR_STVT:
            CSR_SET(BM_CSR_STVT, mask);
            break;
        case BM_CSR_SNXTI:
            CSR_SET(BM_CSR_SNXTI, mask);
            break;
        case BM_CSR_SINTTHRESH:
            CSR_SET(BM_CSR_SINTTHRESH, mask);
            break;
        case BM_CSR_SSCRATCHCSW:
            CSR_SET(BM_CSR_SSCRATCHCSW, mask);
            break;
        case BM_CSR_SSCRATCHCSWL:
            CSR_SET(BM_CSR_SSCRATCHCSWL, mask);
            break;
        case BM_CSR_SINTSTATUS:
            CSR_SET(BM_CSR_SINTSTATUS, mask);
            break;
    #endif
        case BM_CSR_SENVCFG:
            CSR_SET(BM_CSR_SENVCFG, mask);
            break;
#endif
        case BM_CSR_MSTATUS:
            CSR_SET(BM_CSR_MSTATUS, mask);
            break;
        case BM_CSR_MISA:
            CSR_SET(BM_CSR_MISA, mask);
            break;
#if defined(TARGET_EXT_S) || defined(TARGET_EXT_N)
        case BM_CSR_MEDELEG:
            CSR_SET(BM_CSR_MEDELEG, mask);
            break;
        case BM_CSR_MIDELEG:
            CSR_SET(BM_CSR_MIDELEG, mask);
            break;
#endif
        case BM_CSR_MIE:
            CSR_SET(BM_CSR_MIE, mask);
            break;
        case BM_CSR_MTVEC:
            CSR_SET(BM_CSR_MTVEC, mask);
            break;
#ifdef TARGET_HAS_CLIC
        case BM_CSR_MTVT:
            CSR_SET(BM_CSR_MTVT, mask);
            break;
        case BM_CSR_MNXTI:
            CSR_SET(BM_CSR_MNXTI, mask);
            break;
        case BM_CSR_MINTTHRESH:
            CSR_SET(BM_CSR_MINTTHRESH, mask);
            break;
        case BM_CSR_MSCRATCHCSW:
            CSR_SET(BM_CSR_MSCRATCHCSW, mask);
            break;
        case BM_CSR_MSCRATCHCSWL:
            CSR_SET(BM_CSR_MSCRATCHCSWL, mask);
            break;
        case BM_CSR_MINTSTATUS:
            CSR_SET(BM_CSR_MINTSTATUS, mask);
            break;
#endif
        case BM_CSR_MENVCFG:
            CSR_SET(BM_CSR_MENVCFG, mask);
            break;
#if __riscv_xlen == 32
        case BM_CSR_MENVCFGH:
            CSR_SET(BM_CSR_MENVCFGH, mask);
            break;
#endif
        case BM_CSR_MCOUNTEREN:
            CSR_SET(BM_CSR_MCOUNTEREN, mask);
            break;
        case BM_CSR_MCOUNTINHIBIT:
            CSR_SET(BM_CSR_MCOUNTINHIBIT, mask);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMEVENT3:
            CSR_SET(BM_CSR_MHPMEVENT3, mask);
            break;
        case BM_CSR_MHPMEVENT4:
            CSR_SET(BM_CSR_MHPMEVENT4, mask);
            break;
        case BM_CSR_MHPMEVENT5:
            CSR_SET(BM_CSR_MHPMEVENT5, mask);
            break;
        case BM_CSR_MHPMEVENT6:
            CSR_SET(BM_CSR_MHPMEVENT6, mask);
            break;
        case BM_CSR_MHPMEVENT7:
            CSR_SET(BM_CSR_MHPMEVENT7, mask);
            break;
        case BM_CSR_MHPMEVENT8:
            CSR_SET(BM_CSR_MHPMEVENT8, mask);
            break;
        case BM_CSR_MHPMEVENT9:
            CSR_SET(BM_CSR_MHPMEVENT9, mask);
            break;
        case BM_CSR_MHPMEVENT10:
            CSR_SET(BM_CSR_MHPMEVENT10, mask);
            break;
        case BM_CSR_MHPMEVENT11:
            CSR_SET(BM_CSR_MHPMEVENT11, mask);
            break;
        case BM_CSR_MHPMEVENT12:
            CSR_SET(BM_CSR_MHPMEVENT12, mask);
            break;
        case BM_CSR_MHPMEVENT13:
            CSR_SET(BM_CSR_MHPMEVENT13, mask);
            break;
        case BM_CSR_MHPMEVENT14:
            CSR_SET(BM_CSR_MHPMEVENT14, mask);
            break;
        case BM_CSR_MHPMEVENT15:
            CSR_SET(BM_CSR_MHPMEVENT15, mask);
            break;
        case BM_CSR_MHPMEVENT16:
            CSR_SET(BM_CSR_MHPMEVENT16, mask);
            break;
        case BM_CSR_MHPMEVENT17:
            CSR_SET(BM_CSR_MHPMEVENT17, mask);
            break;
        case BM_CSR_MHPMEVENT18:
            CSR_SET(BM_CSR_MHPMEVENT18, mask);
            break;
        case BM_CSR_MHPMEVENT19:
            CSR_SET(BM_CSR_MHPMEVENT19, mask);
            break;
        case BM_CSR_MHPMEVENT20:
            CSR_SET(BM_CSR_MHPMEVENT20, mask);
            break;
        case BM_CSR_MHPMEVENT21:
            CSR_SET(BM_CSR_MHPMEVENT21, mask);
            break;
        case BM_CSR_MHPMEVENT22:
            CSR_SET(BM_CSR_MHPMEVENT22, mask);
            break;
        case BM_CSR_MHPMEVENT23:
            CSR_SET(BM_CSR_MHPMEVENT23, mask);
            break;
        case BM_CSR_MHPMEVENT24:
            CSR_SET(BM_CSR_MHPMEVENT24, mask);
            break;
        case BM_CSR_MHPMEVENT25:
            CSR_SET(BM_CSR_MHPMEVENT25, mask);
            break;
        case BM_CSR_MHPMEVENT26:
            CSR_SET(BM_CSR_MHPMEVENT26, mask);
            break;
        case BM_CSR_MHPMEVENT27:
            CSR_SET(BM_CSR_MHPMEVENT27, mask);
            break;
        case BM_CSR_MHPMEVENT28:
            CSR_SET(BM_CSR_MHPMEVENT28, mask);
            break;
        case BM_CSR_MHPMEVENT29:
            CSR_SET(BM_CSR_MHPMEVENT29, mask);
            break;
        case BM_CSR_MHPMEVENT30:
            CSR_SET(BM_CSR_MHPMEVENT30, mask);
            break;
        case BM_CSR_MHPMEVENT31:
            CSR_SET(BM_CSR_MHPMEVENT31, mask);
            break;
#endif
        case BM_CSR_MSCRATCH:
            CSR_SET(BM_CSR_MSCRATCH, mask);
            break;
        case BM_CSR_MEPC:
            CSR_SET(BM_CSR_MEPC, mask);
            break;
        case BM_CSR_MCAUSE:
            CSR_SET(BM_CSR_MCAUSE, mask);
            break;
        case BM_CSR_MTVAL:
            CSR_SET(BM_CSR_MTVAL, mask);
            break;
        case BM_CSR_MIP:
            CSR_SET(BM_CSR_MIP, mask);
            break;
#ifdef TARGET_HAS_PMP
        case BM_CSR_PMPCFG0:
            CSR_SET(BM_CSR_PMPCFG0, mask);
            break;
    #if TARGET_HAS_PMP_CFG_REG1
        case BM_CSR_PMPCFG1:
            CSR_SET(BM_CSR_PMPCFG1, mask);
            break;
    #endif
    #if TARGET_HAS_PMP_CFG_REG2
        case BM_CSR_PMPCFG2:
            CSR_SET(BM_CSR_PMPCFG2, mask);
            break;
    #endif
    #if TARGET_HAS_PMP_CFG_REG3
        case BM_CSR_PMPCFG3:
            CSR_SET(BM_CSR_PMPCFG3, mask);
            break;
    #endif
        case BM_CSR_PMPADDR0:
            CSR_SET(BM_CSR_PMPADDR0, mask);
            break;
        case BM_CSR_PMPADDR1:
            CSR_SET(BM_CSR_PMPADDR1, mask);
            break;
        case BM_CSR_PMPADDR2:
            CSR_SET(BM_CSR_PMPADDR2, mask);
            break;
        case BM_CSR_PMPADDR3:
            CSR_SET(BM_CSR_PMPADDR3, mask);
            break;
    #if TARGET_PMP_NUM_REGIONS > 4
        case BM_CSR_PMPADDR4:
            CSR_SET(BM_CSR_PMPADDR4, mask);
            break;
        case BM_CSR_PMPADDR5:
            CSR_SET(BM_CSR_PMPADDR5, mask);
            break;
        case BM_CSR_PMPADDR6:
            CSR_SET(BM_CSR_PMPADDR6, mask);
            break;
        case BM_CSR_PMPADDR7:
            CSR_SET(BM_CSR_PMPADDR7, mask);
            break;
    #endif
    #if TARGET_PMP_NUM_REGIONS > 8
        case BM_CSR_PMPADDR8:
            CSR_SET(BM_CSR_PMPADDR8, mask);
            break;
        case BM_CSR_PMPADDR9:
            CSR_SET(BM_CSR_PMPADDR9, mask);
            break;
        case BM_CSR_PMPADDR10:
            CSR_SET(BM_CSR_PMPADDR10, mask);
            break;
        case BM_CSR_PMPADDR11:
            CSR_SET(BM_CSR_PMPADDR11, mask);
            break;
    #endif
    #if TARGET_PMP_NUM_REGIONS > 12
        case BM_CSR_PMPADDR12:
            CSR_SET(BM_CSR_PMPADDR12, mask);
            break;
        case BM_CSR_PMPADDR13:
            CSR_SET(BM_CSR_PMPADDR13, mask);
            break;
        case BM_CSR_PMPADDR14:
            CSR_SET(BM_CSR_PMPADDR14, mask);
            break;
        case BM_CSR_PMPADDR15:
            CSR_SET(BM_CSR_PMPADDR15, mask);
            break;
    #endif
#endif
        case BM_CSR_MSECCFG:
            CSR_SET(BM_CSR_MSECCFG, mask);
            break;
#if __riscv_xlen == 32
        case BM_CSR_MSECCFGH:
            CSR_SET(BM_CSR_MSECCFGH, mask);
            break;
#endif
        case BM_CSR_TSELECT:
            CSR_SET(BM_CSR_TSELECT, mask);
            break;
        case BM_CSR_TDATA1:
            CSR_SET(BM_CSR_TDATA1, mask);
            break;
        case BM_CSR_TDATA2:
            CSR_SET(BM_CSR_TDATA2, mask);
            break;
        case BM_CSR_TDATA3:
            CSR_SET(BM_CSR_TDATA3, mask);
            break;
        case BM_CSR_TINFO:
            CSR_SET(BM_CSR_TINFO, mask);
            break;
        case BM_CSR_TCONTROL:
            CSR_SET(BM_CSR_TCONTROL, mask);
            break;
        case BM_CSR_MCONTEXT:
            CSR_SET(BM_CSR_MCONTEXT, mask);
            break;
#ifdef TARGET_HAS_PIC
        case BM_CSR_MPICFLAG:
            CSR_SET(BM_CSR_MPICFLAG, mask);
            break;
        case BM_CSR_MPICMASK:
            CSR_SET(BM_CSR_MPICMASK, mask);
            break;
    #if TARGET_PIC_NUM_INTERRUPTS > 32
        case BM_CSR_MPICFLAG1:
            CSR_SET(BM_CSR_MPICFLAG1, mask);
            break;
        case BM_CSR_MPICMASK1:
            CSR_SET(BM_CSR_MPICMASK1, mask);
            break;
    #endif
    #if TARGET_PIC_NUM_INTERRUPTS > 64
        case BM_CSR_MPICFLAG2:
            CSR_SET(BM_CSR_MPICFLAG2, mask);
            break;
        case BM_CSR_MPICMASK2:
            CSR_SET(BM_CSR_MPICMASK2, mask);
            break;
    #endif
    #if TARGET_PIC_NUM_INTERRUPTS > 96
        case BM_CSR_MPICFLAG3:
            CSR_SET(BM_CSR_MPICFLAG3, mask);
            break;
        case BM_CSR_MPICMASK3:
            CSR_SET(BM_CSR_MPICMASK3, mask);
            break;
    #endif
#endif
        case BM_CSR_MCYCLE:
            CSR_SET(BM_CSR_MCYCLE, mask);
            break;
        case BM_CSR_MINSTRET:
            CSR_SET(BM_CSR_MINSTRET, mask);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMCOUNTER3:
            CSR_SET(BM_CSR_MHPMCOUNTER3, mask);
            break;
        case BM_CSR_MHPMCOUNTER4:
            CSR_SET(BM_CSR_MHPMCOUNTER4, mask);
            break;
        case BM_CSR_MHPMCOUNTER5:
            CSR_SET(BM_CSR_MHPMCOUNTER5, mask);
            break;
        case BM_CSR_MHPMCOUNTER6:
            CSR_SET(BM_CSR_MHPMCOUNTER6, mask);
            break;
        case BM_CSR_MHPMCOUNTER7:
            CSR_SET(BM_CSR_MHPMCOUNTER7, mask);
            break;
        case BM_CSR_MHPMCOUNTER8:
            CSR_SET(BM_CSR_MHPMCOUNTER8, mask);
            break;
        case BM_CSR_MHPMCOUNTER9:
            CSR_SET(BM_CSR_MHPMCOUNTER9, mask);
            break;
        case BM_CSR_MHPMCOUNTER10:
            CSR_SET(BM_CSR_MHPMCOUNTER10, mask);
            break;
        case BM_CSR_MHPMCOUNTER11:
            CSR_SET(BM_CSR_MHPMCOUNTER11, mask);
            break;
        case BM_CSR_MHPMCOUNTER12:
            CSR_SET(BM_CSR_MHPMCOUNTER12, mask);
            break;
        case BM_CSR_MHPMCOUNTER13:
            CSR_SET(BM_CSR_MHPMCOUNTER13, mask);
            break;
        case BM_CSR_MHPMCOUNTER14:
            CSR_SET(BM_CSR_MHPMCOUNTER14, mask);
            break;
        case BM_CSR_MHPMCOUNTER15:
            CSR_SET(BM_CSR_MHPMCOUNTER15, mask);
            break;
        case BM_CSR_MHPMCOUNTER16:
            CSR_SET(BM_CSR_MHPMCOUNTER16, mask);
            break;
        case BM_CSR_MHPMCOUNTER17:
            CSR_SET(BM_CSR_MHPMCOUNTER17, mask);
            break;
        case BM_CSR_MHPMCOUNTER18:
            CSR_SET(BM_CSR_MHPMCOUNTER18, mask);
            break;
        case BM_CSR_MHPMCOUNTER19:
            CSR_SET(BM_CSR_MHPMCOUNTER19, mask);
            break;
        case BM_CSR_MHPMCOUNTER20:
            CSR_SET(BM_CSR_MHPMCOUNTER20, mask);
            break;
        case BM_CSR_MHPMCOUNTER21:
            CSR_SET(BM_CSR_MHPMCOUNTER21, mask);
            break;
        case BM_CSR_MHPMCOUNTER22:
            CSR_SET(BM_CSR_MHPMCOUNTER22, mask);
            break;
        case BM_CSR_MHPMCOUNTER23:
            CSR_SET(BM_CSR_MHPMCOUNTER23, mask);
            break;
        case BM_CSR_MHPMCOUNTER24:
            CSR_SET(BM_CSR_MHPMCOUNTER24, mask);
            break;
        case BM_CSR_MHPMCOUNTER25:
            CSR_SET(BM_CSR_MHPMCOUNTER25, mask);
            break;
        case BM_CSR_MHPMCOUNTER26:
            CSR_SET(BM_CSR_MHPMCOUNTER26, mask);
            break;
        case BM_CSR_MHPMCOUNTER27:
            CSR_SET(BM_CSR_MHPMCOUNTER27, mask);
            break;
        case BM_CSR_MHPMCOUNTER28:
            CSR_SET(BM_CSR_MHPMCOUNTER28, mask);
            break;
        case BM_CSR_MHPMCOUNTER29:
            CSR_SET(BM_CSR_MHPMCOUNTER29, mask);
            break;
        case BM_CSR_MHPMCOUNTER30:
            CSR_SET(BM_CSR_MHPMCOUNTER30, mask);
            break;
        case BM_CSR_MHPMCOUNTER31:
            CSR_SET(BM_CSR_MHPMCOUNTER31, mask);
            break;
#endif
#if __riscv_xlen == 32
        case BM_CSR_MCYCLEH:
            CSR_SET(BM_CSR_MCYCLEH, mask);
            break;
        case BM_CSR_MINSTRETH:
            CSR_SET(BM_CSR_MINSTRETH, mask);
            break;
    #ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMCOUNTER3H:
            CSR_SET(BM_CSR_MHPMCOUNTER3H, mask);
            break;
        case BM_CSR_MHPMCOUNTER4H:
            CSR_SET(BM_CSR_MHPMCOUNTER4H, mask);
            break;
        case BM_CSR_MHPMCOUNTER5H:
            CSR_SET(BM_CSR_MHPMCOUNTER5H, mask);
            break;
        case BM_CSR_MHPMCOUNTER6H:
            CSR_SET(BM_CSR_MHPMCOUNTER6H, mask);
            break;
        case BM_CSR_MHPMCOUNTER7H:
            CSR_SET(BM_CSR_MHPMCOUNTER7H, mask);
            break;
        case BM_CSR_MHPMCOUNTER8H:
            CSR_SET(BM_CSR_MHPMCOUNTER8H, mask);
            break;
        case BM_CSR_MHPMCOUNTER9H:
            CSR_SET(BM_CSR_MHPMCOUNTER9H, mask);
            break;
        case BM_CSR_MHPMCOUNTER10H:
            CSR_SET(BM_CSR_MHPMCOUNTER10H, mask);
            break;
        case BM_CSR_MHPMCOUNTER11H:
            CSR_SET(BM_CSR_MHPMCOUNTER11H, mask);
            break;
        case BM_CSR_MHPMCOUNTER12H:
            CSR_SET(BM_CSR_MHPMCOUNTER12H, mask);
            break;
        case BM_CSR_MHPMCOUNTER13H:
            CSR_SET(BM_CSR_MHPMCOUNTER13H, mask);
            break;
        case BM_CSR_MHPMCOUNTER14H:
            CSR_SET(BM_CSR_MHPMCOUNTER14H, mask);
            break;
        case BM_CSR_MHPMCOUNTER15H:
            CSR_SET(BM_CSR_MHPMCOUNTER15H, mask);
            break;
        case BM_CSR_MHPMCOUNTER16H:
            CSR_SET(BM_CSR_MHPMCOUNTER16H, mask);
            break;
        case BM_CSR_MHPMCOUNTER17H:
            CSR_SET(BM_CSR_MHPMCOUNTER17H, mask);
            break;
        case BM_CSR_MHPMCOUNTER18H:
            CSR_SET(BM_CSR_MHPMCOUNTER18H, mask);
            break;
        case BM_CSR_MHPMCOUNTER19H:
            CSR_SET(BM_CSR_MHPMCOUNTER19H, mask);
            break;
        case BM_CSR_MHPMCOUNTER20H:
            CSR_SET(BM_CSR_MHPMCOUNTER20H, mask);
            break;
        case BM_CSR_MHPMCOUNTER21H:
            CSR_SET(BM_CSR_MHPMCOUNTER21H, mask);
            break;
        case BM_CSR_MHPMCOUNTER22H:
            CSR_SET(BM_CSR_MHPMCOUNTER22H, mask);
            break;
        case BM_CSR_MHPMCOUNTER23H:
            CSR_SET(BM_CSR_MHPMCOUNTER23H, mask);
            break;
        case BM_CSR_MHPMCOUNTER24H:
            CSR_SET(BM_CSR_MHPMCOUNTER24H, mask);
            break;
        case BM_CSR_MHPMCOUNTER25H:
            CSR_SET(BM_CSR_MHPMCOUNTER25H, mask);
            break;
        case BM_CSR_MHPMCOUNTER26H:
            CSR_SET(BM_CSR_MHPMCOUNTER26H, mask);
            break;
        case BM_CSR_MHPMCOUNTER27H:
            CSR_SET(BM_CSR_MHPMCOUNTER27H, mask);
            break;
        case BM_CSR_MHPMCOUNTER28H:
            CSR_SET(BM_CSR_MHPMCOUNTER28H, mask);
            break;
        case BM_CSR_MHPMCOUNTER29H:
            CSR_SET(BM_CSR_MHPMCOUNTER29H, mask);
            break;
        case BM_CSR_MHPMCOUNTER30H:
            CSR_SET(BM_CSR_MHPMCOUNTER30H, mask);
            break;
        case BM_CSR_MHPMCOUNTER31H:
            CSR_SET(BM_CSR_MHPMCOUNTER31H, mask);
            break;
    #endif
#endif
        case BM_CSR_CYCLE:
            CSR_SET(BM_CSR_CYCLE, mask);
            break;
        case BM_CSR_TIME:
            CSR_SET(BM_CSR_TIME, mask);
            break;
        case BM_CSR_INSTRET:
            CSR_SET(BM_CSR_INSTRET, mask);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3:
            CSR_SET(BM_CSR_HPMCOUNTER3, mask);
            break;
        case BM_CSR_HPMCOUNTER4:
            CSR_SET(BM_CSR_HPMCOUNTER4, mask);
            break;
        case BM_CSR_HPMCOUNTER5:
            CSR_SET(BM_CSR_HPMCOUNTER5, mask);
            break;
        case BM_CSR_HPMCOUNTER6:
            CSR_SET(BM_CSR_HPMCOUNTER6, mask);
            break;
        case BM_CSR_HPMCOUNTER7:
            CSR_SET(BM_CSR_HPMCOUNTER7, mask);
            break;
        case BM_CSR_HPMCOUNTER8:
            CSR_SET(BM_CSR_HPMCOUNTER8, mask);
            break;
        case BM_CSR_HPMCOUNTER9:
            CSR_SET(BM_CSR_HPMCOUNTER9, mask);
            break;
        case BM_CSR_HPMCOUNTER10:
            CSR_SET(BM_CSR_HPMCOUNTER10, mask);
            break;
        case BM_CSR_HPMCOUNTER11:
            CSR_SET(BM_CSR_HPMCOUNTER11, mask);
            break;
        case BM_CSR_HPMCOUNTER12:
            CSR_SET(BM_CSR_HPMCOUNTER12, mask);
            break;
        case BM_CSR_HPMCOUNTER13:
            CSR_SET(BM_CSR_HPMCOUNTER13, mask);
            break;
        case BM_CSR_HPMCOUNTER14:
            CSR_SET(BM_CSR_HPMCOUNTER14, mask);
            break;
        case BM_CSR_HPMCOUNTER15:
            CSR_SET(BM_CSR_HPMCOUNTER15, mask);
            break;
        case BM_CSR_HPMCOUNTER16:
            CSR_SET(BM_CSR_HPMCOUNTER16, mask);
            break;
        case BM_CSR_HPMCOUNTER17:
            CSR_SET(BM_CSR_HPMCOUNTER17, mask);
            break;
        case BM_CSR_HPMCOUNTER18:
            CSR_SET(BM_CSR_HPMCOUNTER18, mask);
            break;
        case BM_CSR_HPMCOUNTER19:
            CSR_SET(BM_CSR_HPMCOUNTER19, mask);
            break;
        case BM_CSR_HPMCOUNTER20:
            CSR_SET(BM_CSR_HPMCOUNTER20, mask);
            break;
        case BM_CSR_HPMCOUNTER21:
            CSR_SET(BM_CSR_HPMCOUNTER21, mask);
            break;
        case BM_CSR_HPMCOUNTER22:
            CSR_SET(BM_CSR_HPMCOUNTER22, mask);
            break;
        case BM_CSR_HPMCOUNTER23:
            CSR_SET(BM_CSR_HPMCOUNTER23, mask);
            break;
        case BM_CSR_HPMCOUNTER24:
            CSR_SET(BM_CSR_HPMCOUNTER24, mask);
            break;
        case BM_CSR_HPMCOUNTER25:
            CSR_SET(BM_CSR_HPMCOUNTER25, mask);
            break;
        case BM_CSR_HPMCOUNTER26:
            CSR_SET(BM_CSR_HPMCOUNTER26, mask);
            break;
        case BM_CSR_HPMCOUNTER27:
            CSR_SET(BM_CSR_HPMCOUNTER27, mask);
            break;
        case BM_CSR_HPMCOUNTER28:
            CSR_SET(BM_CSR_HPMCOUNTER28, mask);
            break;
        case BM_CSR_HPMCOUNTER29:
            CSR_SET(BM_CSR_HPMCOUNTER29, mask);
            break;
        case BM_CSR_HPMCOUNTER30:
            CSR_SET(BM_CSR_HPMCOUNTER30, mask);
            break;
        case BM_CSR_HPMCOUNTER31:
            CSR_SET(BM_CSR_HPMCOUNTER31, mask);
            break;
#endif
#if __riscv_xlen == 32
        case BM_CSR_CYCLEH:
            CSR_SET(BM_CSR_CYCLEH, mask);
            break;
        case BM_CSR_TIMEH:
            CSR_SET(BM_CSR_TIMEH, mask);
            break;
        case BM_CSR_INSTRETH:
            CSR_SET(BM_CSR_INSTRETH, mask);
            break;
    #ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3H:
            CSR_SET(BM_CSR_HPMCOUNTER3H, mask);
            break;
        case BM_CSR_HPMCOUNTER4H:
            CSR_SET(BM_CSR_HPMCOUNTER4H, mask);
            break;
        case BM_CSR_HPMCOUNTER5H:
            CSR_SET(BM_CSR_HPMCOUNTER5H, mask);
            break;
        case BM_CSR_HPMCOUNTER6H:
            CSR_SET(BM_CSR_HPMCOUNTER6H, mask);
            break;
        case BM_CSR_HPMCOUNTER7H:
            CSR_SET(BM_CSR_HPMCOUNTER7H, mask);
            break;
        case BM_CSR_HPMCOUNTER8H:
            CSR_SET(BM_CSR_HPMCOUNTER8H, mask);
            break;
        case BM_CSR_HPMCOUNTER9H:
            CSR_SET(BM_CSR_HPMCOUNTER9H, mask);
            break;
        case BM_CSR_HPMCOUNTER10H:
            CSR_SET(BM_CSR_HPMCOUNTER10H, mask);
            break;
        case BM_CSR_HPMCOUNTER11H:
            CSR_SET(BM_CSR_HPMCOUNTER11H, mask);
            break;
        case BM_CSR_HPMCOUNTER12H:
            CSR_SET(BM_CSR_HPMCOUNTER12H, mask);
            break;
        case BM_CSR_HPMCOUNTER13H:
            CSR_SET(BM_CSR_HPMCOUNTER13H, mask);
            break;
        case BM_CSR_HPMCOUNTER14H:
            CSR_SET(BM_CSR_HPMCOUNTER14H, mask);
            break;
        case BM_CSR_HPMCOUNTER15H:
            CSR_SET(BM_CSR_HPMCOUNTER15H, mask);
            break;
        case BM_CSR_HPMCOUNTER16H:
            CSR_SET(BM_CSR_HPMCOUNTER16H, mask);
            break;
        case BM_CSR_HPMCOUNTER17H:
            CSR_SET(BM_CSR_HPMCOUNTER17H, mask);
            break;
        case BM_CSR_HPMCOUNTER18H:
            CSR_SET(BM_CSR_HPMCOUNTER18H, mask);
            break;
        case BM_CSR_HPMCOUNTER19H:
            CSR_SET(BM_CSR_HPMCOUNTER19H, mask);
            break;
        case BM_CSR_HPMCOUNTER20H:
            CSR_SET(BM_CSR_HPMCOUNTER20H, mask);
            break;
        case BM_CSR_HPMCOUNTER21H:
            CSR_SET(BM_CSR_HPMCOUNTER21H, mask);
            break;
        case BM_CSR_HPMCOUNTER22H:
            CSR_SET(BM_CSR_HPMCOUNTER22H, mask);
            break;
        case BM_CSR_HPMCOUNTER23H:
            CSR_SET(BM_CSR_HPMCOUNTER23H, mask);
            break;
        case BM_CSR_HPMCOUNTER24H:
            CSR_SET(BM_CSR_HPMCOUNTER24H, mask);
            break;
        case BM_CSR_HPMCOUNTER25H:
            CSR_SET(BM_CSR_HPMCOUNTER25H, mask);
            break;
        case BM_CSR_HPMCOUNTER26H:
            CSR_SET(BM_CSR_HPMCOUNTER26H, mask);
            break;
        case BM_CSR_HPMCOUNTER27H:
            CSR_SET(BM_CSR_HPMCOUNTER27H, mask);
            break;
        case BM_CSR_HPMCOUNTER28H:
            CSR_SET(BM_CSR_HPMCOUNTER28H, mask);
            break;
        case BM_CSR_HPMCOUNTER29H:
            CSR_SET(BM_CSR_HPMCOUNTER29H, mask);
            break;
        case BM_CSR_HPMCOUNTER30H:
            CSR_SET(BM_CSR_HPMCOUNTER30H, mask);
            break;
        case BM_CSR_HPMCOUNTER31H:
            CSR_SET(BM_CSR_HPMCOUNTER31H, mask);
            break;
    #endif
#endif
        case BM_CSR_MVENDORID:
            CSR_SET(BM_CSR_MVENDORID, mask);
            break;
        case BM_CSR_MARCHID:
            CSR_SET(BM_CSR_MARCHID, mask);
            break;
        case BM_CSR_MIMPID:
            CSR_SET(BM_CSR_MIMPID, mask);
            break;
        case BM_CSR_MHARTID:
            CSR_SET(BM_CSR_MHARTID, mask);
            break;
        default:
#ifdef TARGET_HAS_CUSTOM_CSR
            target_csr_set_mask(csr, mask);
#else
            bm_error("Unsupported CSR.");
#endif
            break;
    }
}

void bm_csr_clear_mask(const int csr, const xlen_t mask)
{
    switch (csr)
    {
#ifdef TARGET_EXT_N
        case BM_CSR_USTATUS:
            CSR_CLEAR(BM_CSR_USTATUS, mask);
            break;
#endif
#ifdef __riscv_flen
        case BM_CSR_FFLAGS:
            CSR_CLEAR(BM_CSR_FFLAGS, mask);
            break;
        case BM_CSR_FRM:
            CSR_CLEAR(BM_CSR_FRM, mask);
            break;
        case BM_CSR_FCSR:
            CSR_CLEAR(BM_CSR_FCSR, mask);
            break;
#endif
#ifdef TARGET_EXT_N
        case BM_CSR_UIE:
            CSR_CLEAR(BM_CSR_UIE, mask);
            break;
        case BM_CSR_UTVEC:
            CSR_CLEAR(BM_CSR_UTVEC, mask);
            break;
        case BM_CSR_USCRATCH:
            CSR_CLEAR(BM_CSR_USCRATCH, mask);
            break;
        case BM_CSR_UEPC:
            CSR_CLEAR(BM_CSR_UEPC, mask);
            break;
        case BM_CSR_UCAUSE:
            CSR_CLEAR(BM_CSR_UCAUSE, mask);
            break;
        case BM_CSR_UTVAL:
            CSR_CLEAR(BM_CSR_UTVAL, mask);
            break;
        case BM_CSR_UIP:
            CSR_CLEAR(BM_CSR_UIP, mask);
            break;
#endif
        case BM_CSR_SEED:
            CSR_CLEAR(BM_CSR_SEED, mask);
            break;
#ifdef TARGET_EXT_S
        case BM_CSR_SSTATUS:
            CSR_CLEAR(BM_CSR_SSTATUS, mask);
            break;
    #ifdef TARGET_EXT_N
        case BM_CSR_SEDELEG:
            CSR_CLEAR(BM_CSR_SEDELEG, mask);
            break;
        case BM_CSR_SIDELEG:
            CSR_CLEAR(BM_CSR_SIDELEG, mask);
            break;
    #endif
        case BM_CSR_SIE:
            CSR_CLEAR(BM_CSR_SIE, mask);
            break;
        case BM_CSR_STVEC:
            CSR_CLEAR(BM_CSR_STVEC, mask);
            break;
        case BM_CSR_SCOUNTEREN:
            CSR_CLEAR(BM_CSR_SCOUNTEREN, mask);
            break;
        case BM_CSR_SSCRATCH:
            CSR_CLEAR(BM_CSR_SSCRATCH, mask);
            break;
        case BM_CSR_SEPC:
            CSR_CLEAR(BM_CSR_SEPC, mask);
            break;
        case BM_CSR_SCAUSE:
            CSR_CLEAR(BM_CSR_SCAUSE, mask);
            break;
        case BM_CSR_STVAL:
            CSR_CLEAR(BM_CSR_STVAL, mask);
            break;
        case BM_CSR_SIP:
            CSR_CLEAR(BM_CSR_SIP, mask);
            break;
        case BM_CSR_SATP:
            CSR_CLEAR(BM_CSR_SATP, mask);
            break;
    #ifdef TARGET_HAS_CLIC
        case BM_CSR_STVT:
            CSR_CLEAR(BM_CSR_STVT, mask);
            break;
        case BM_CSR_SNXTI:
            CSR_CLEAR(BM_CSR_SNXTI, mask);
            break;
        case BM_CSR_SINTTHRESH:
            CSR_CLEAR(BM_CSR_SINTTHRESH, mask);
            break;
        case BM_CSR_SSCRATCHCSW:
            CSR_CLEAR(BM_CSR_SSCRATCHCSW, mask);
            break;
        case BM_CSR_SSCRATCHCSWL:
            CSR_CLEAR(BM_CSR_SSCRATCHCSWL, mask);
            break;
        case BM_CSR_SINTSTATUS:
            CSR_CLEAR(BM_CSR_SINTSTATUS, mask);
            break;
    #endif
        case BM_CSR_SENVCFG:
            CSR_CLEAR(BM_CSR_SENVCFG, mask);
            break;
#endif
        case BM_CSR_MSTATUS:
            CSR_CLEAR(BM_CSR_MSTATUS, mask);
            break;
        case BM_CSR_MISA:
            CSR_CLEAR(BM_CSR_MISA, mask);
            break;
#if defined(TARGET_EXT_S) || defined(TARGET_EXT_N)
        case BM_CSR_MEDELEG:
            CSR_CLEAR(BM_CSR_MEDELEG, mask);
            break;
        case BM_CSR_MIDELEG:
            CSR_CLEAR(BM_CSR_MIDELEG, mask);
            break;
#endif
        case BM_CSR_MIE:
            CSR_CLEAR(BM_CSR_MIE, mask);
            break;
        case BM_CSR_MTVEC:
            CSR_CLEAR(BM_CSR_MTVEC, mask);
            break;
#ifdef TARGET_HAS_CLIC
        case BM_CSR_MTVT:
            CSR_CLEAR(BM_CSR_MTVT, mask);
            break;
        case BM_CSR_MNXTI:
            CSR_CLEAR(BM_CSR_MNXTI, mask);
            break;
        case BM_CSR_MINTTHRESH:
            CSR_CLEAR(BM_CSR_MINTTHRESH, mask);
            break;
        case BM_CSR_MSCRATCHCSW:
            CSR_CLEAR(BM_CSR_MSCRATCHCSW, mask);
            break;
        case BM_CSR_MSCRATCHCSWL:
            CSR_CLEAR(BM_CSR_MSCRATCHCSWL, mask);
            break;
        case BM_CSR_MINTSTATUS:
            CSR_CLEAR(BM_CSR_MINTSTATUS, mask);
            break;
#endif
        case BM_CSR_MENVCFG:
            CSR_CLEAR(BM_CSR_MENVCFG, mask);
            break;
#if __riscv_xlen == 32
        case BM_CSR_MENVCFGH:
            CSR_CLEAR(BM_CSR_MENVCFGH, mask);
            break;
#endif
        case BM_CSR_MCOUNTEREN:
            CSR_CLEAR(BM_CSR_MCOUNTEREN, mask);
            break;
        case BM_CSR_MCOUNTINHIBIT:
            CSR_CLEAR(BM_CSR_MCOUNTINHIBIT, mask);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMEVENT3:
            CSR_CLEAR(BM_CSR_MHPMEVENT3, mask);
            break;
        case BM_CSR_MHPMEVENT4:
            CSR_CLEAR(BM_CSR_MHPMEVENT4, mask);
            break;
        case BM_CSR_MHPMEVENT5:
            CSR_CLEAR(BM_CSR_MHPMEVENT5, mask);
            break;
        case BM_CSR_MHPMEVENT6:
            CSR_CLEAR(BM_CSR_MHPMEVENT6, mask);
            break;
        case BM_CSR_MHPMEVENT7:
            CSR_CLEAR(BM_CSR_MHPMEVENT7, mask);
            break;
        case BM_CSR_MHPMEVENT8:
            CSR_CLEAR(BM_CSR_MHPMEVENT8, mask);
            break;
        case BM_CSR_MHPMEVENT9:
            CSR_CLEAR(BM_CSR_MHPMEVENT9, mask);
            break;
        case BM_CSR_MHPMEVENT10:
            CSR_CLEAR(BM_CSR_MHPMEVENT10, mask);
            break;
        case BM_CSR_MHPMEVENT11:
            CSR_CLEAR(BM_CSR_MHPMEVENT11, mask);
            break;
        case BM_CSR_MHPMEVENT12:
            CSR_CLEAR(BM_CSR_MHPMEVENT12, mask);
            break;
        case BM_CSR_MHPMEVENT13:
            CSR_CLEAR(BM_CSR_MHPMEVENT13, mask);
            break;
        case BM_CSR_MHPMEVENT14:
            CSR_CLEAR(BM_CSR_MHPMEVENT14, mask);
            break;
        case BM_CSR_MHPMEVENT15:
            CSR_CLEAR(BM_CSR_MHPMEVENT15, mask);
            break;
        case BM_CSR_MHPMEVENT16:
            CSR_CLEAR(BM_CSR_MHPMEVENT16, mask);
            break;
        case BM_CSR_MHPMEVENT17:
            CSR_CLEAR(BM_CSR_MHPMEVENT17, mask);
            break;
        case BM_CSR_MHPMEVENT18:
            CSR_CLEAR(BM_CSR_MHPMEVENT18, mask);
            break;
        case BM_CSR_MHPMEVENT19:
            CSR_CLEAR(BM_CSR_MHPMEVENT19, mask);
            break;
        case BM_CSR_MHPMEVENT20:
            CSR_CLEAR(BM_CSR_MHPMEVENT20, mask);
            break;
        case BM_CSR_MHPMEVENT21:
            CSR_CLEAR(BM_CSR_MHPMEVENT21, mask);
            break;
        case BM_CSR_MHPMEVENT22:
            CSR_CLEAR(BM_CSR_MHPMEVENT22, mask);
            break;
        case BM_CSR_MHPMEVENT23:
            CSR_CLEAR(BM_CSR_MHPMEVENT23, mask);
            break;
        case BM_CSR_MHPMEVENT24:
            CSR_CLEAR(BM_CSR_MHPMEVENT24, mask);
            break;
        case BM_CSR_MHPMEVENT25:
            CSR_CLEAR(BM_CSR_MHPMEVENT25, mask);
            break;
        case BM_CSR_MHPMEVENT26:
            CSR_CLEAR(BM_CSR_MHPMEVENT26, mask);
            break;
        case BM_CSR_MHPMEVENT27:
            CSR_CLEAR(BM_CSR_MHPMEVENT27, mask);
            break;
        case BM_CSR_MHPMEVENT28:
            CSR_CLEAR(BM_CSR_MHPMEVENT28, mask);
            break;
        case BM_CSR_MHPMEVENT29:
            CSR_CLEAR(BM_CSR_MHPMEVENT29, mask);
            break;
        case BM_CSR_MHPMEVENT30:
            CSR_CLEAR(BM_CSR_MHPMEVENT30, mask);
            break;
        case BM_CSR_MHPMEVENT31:
            CSR_CLEAR(BM_CSR_MHPMEVENT31, mask);
            break;
#endif
        case BM_CSR_MSCRATCH:
            CSR_CLEAR(BM_CSR_MSCRATCH, mask);
            break;
        case BM_CSR_MEPC:
            CSR_CLEAR(BM_CSR_MEPC, mask);
            break;
        case BM_CSR_MCAUSE:
            CSR_CLEAR(BM_CSR_MCAUSE, mask);
            break;
        case BM_CSR_MTVAL:
            CSR_CLEAR(BM_CSR_MTVAL, mask);
            break;
        case BM_CSR_MIP:
            CSR_CLEAR(BM_CSR_MIP, mask);
            break;
#ifdef TARGET_HAS_PMP
        case BM_CSR_PMPCFG0:
            CSR_CLEAR(BM_CSR_PMPCFG0, mask);
            break;
    #if TARGET_HAS_PMP_CFG_REG1
        case BM_CSR_PMPCFG1:
            CSR_CLEAR(BM_CSR_PMPCFG1, mask);
            break;
    #endif
    #if TARGET_HAS_PMP_CFG_REG2
        case BM_CSR_PMPCFG2:
            CSR_CLEAR(BM_CSR_PMPCFG2, mask);
            break;
    #endif
    #if TARGET_HAS_PMP_CFG_REG3
        case BM_CSR_PMPCFG3:
            CSR_CLEAR(BM_CSR_PMPCFG3, mask);
            break;
    #endif
        case BM_CSR_PMPADDR0:
            CSR_CLEAR(BM_CSR_PMPADDR0, mask);
            break;
        case BM_CSR_PMPADDR1:
            CSR_CLEAR(BM_CSR_PMPADDR1, mask);
            break;
        case BM_CSR_PMPADDR2:
            CSR_CLEAR(BM_CSR_PMPADDR2, mask);
            break;
        case BM_CSR_PMPADDR3:
            CSR_CLEAR(BM_CSR_PMPADDR3, mask);
            break;
    #if TARGET_PMP_NUM_REGIONS > 4
        case BM_CSR_PMPADDR4:
            CSR_CLEAR(BM_CSR_PMPADDR4, mask);
            break;
        case BM_CSR_PMPADDR5:
            CSR_CLEAR(BM_CSR_PMPADDR5, mask);
            break;
        case BM_CSR_PMPADDR6:
            CSR_CLEAR(BM_CSR_PMPADDR6, mask);
            break;
        case BM_CSR_PMPADDR7:
            CSR_CLEAR(BM_CSR_PMPADDR7, mask);
            break;
    #endif
    #if TARGET_PMP_NUM_REGIONS > 8
        case BM_CSR_PMPADDR8:
            CSR_CLEAR(BM_CSR_PMPADDR8, mask);
            break;
        case BM_CSR_PMPADDR9:
            CSR_CLEAR(BM_CSR_PMPADDR9, mask);
            break;
        case BM_CSR_PMPADDR10:
            CSR_CLEAR(BM_CSR_PMPADDR10, mask);
            break;
        case BM_CSR_PMPADDR11:
            CSR_CLEAR(BM_CSR_PMPADDR11, mask);
            break;
    #endif
    #if TARGET_PMP_NUM_REGIONS > 12
        case BM_CSR_PMPADDR12:
            CSR_CLEAR(BM_CSR_PMPADDR12, mask);
            break;
        case BM_CSR_PMPADDR13:
            CSR_CLEAR(BM_CSR_PMPADDR13, mask);
            break;
        case BM_CSR_PMPADDR14:
            CSR_CLEAR(BM_CSR_PMPADDR14, mask);
            break;
        case BM_CSR_PMPADDR15:
            CSR_CLEAR(BM_CSR_PMPADDR15, mask);
            break;
    #endif
#endif
        case BM_CSR_MSECCFG:
            CSR_CLEAR(BM_CSR_MSECCFG, mask);
            break;
#if __riscv_xlen == 32
        case BM_CSR_MSECCFGH:
            CSR_CLEAR(BM_CSR_MSECCFGH, mask);
            break;
#endif
        case BM_CSR_TSELECT:
            CSR_CLEAR(BM_CSR_TSELECT, mask);
            break;
        case BM_CSR_TDATA1:
            CSR_CLEAR(BM_CSR_TDATA1, mask);
            break;
        case BM_CSR_TDATA2:
            CSR_CLEAR(BM_CSR_TDATA2, mask);
            break;
        case BM_CSR_TDATA3:
            CSR_CLEAR(BM_CSR_TDATA3, mask);
            break;
        case BM_CSR_TINFO:
            CSR_CLEAR(BM_CSR_TINFO, mask);
            break;
        case BM_CSR_TCONTROL:
            CSR_CLEAR(BM_CSR_TCONTROL, mask);
            break;
        case BM_CSR_MCONTEXT:
            CSR_CLEAR(BM_CSR_MCONTEXT, mask);
            break;
#ifdef TARGET_HAS_PIC
        case BM_CSR_MPICFLAG:
            CSR_CLEAR(BM_CSR_MPICFLAG, mask);
            break;
        case BM_CSR_MPICMASK:
            CSR_CLEAR(BM_CSR_MPICMASK, mask);
            break;
    #if TARGET_PIC_NUM_INTERRUPTS > 32
        case BM_CSR_MPICFLAG1:
            CSR_CLEAR(BM_CSR_MPICFLAG1, mask);
            break;
        case BM_CSR_MPICMASK1:
            CSR_CLEAR(BM_CSR_MPICMASK1, mask);
            break;
    #endif
    #if TARGET_PIC_NUM_INTERRUPTS > 64
        case BM_CSR_MPICFLAG2:
            CSR_CLEAR(BM_CSR_MPICFLAG2, mask);
            break;
        case BM_CSR_MPICMASK2:
            CSR_CLEAR(BM_CSR_MPICMASK2, mask);
            break;
    #endif
    #if TARGET_PIC_NUM_INTERRUPTS > 96
        case BM_CSR_MPICFLAG3:
            CSR_CLEAR(BM_CSR_MPICFLAG3, mask);
            break;
        case BM_CSR_MPICMASK3:
            CSR_CLEAR(BM_CSR_MPICMASK3, mask);
            break;
    #endif
#endif
        case BM_CSR_MCYCLE:
            CSR_CLEAR(BM_CSR_MCYCLE, mask);
            break;
        case BM_CSR_MINSTRET:
            CSR_CLEAR(BM_CSR_MINSTRET, mask);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMCOUNTER3:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER3, mask);
            break;
        case BM_CSR_MHPMCOUNTER4:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER4, mask);
            break;
        case BM_CSR_MHPMCOUNTER5:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER5, mask);
            break;
        case BM_CSR_MHPMCOUNTER6:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER6, mask);
            break;
        case BM_CSR_MHPMCOUNTER7:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER7, mask);
            break;
        case BM_CSR_MHPMCOUNTER8:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER8, mask);
            break;
        case BM_CSR_MHPMCOUNTER9:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER9, mask);
            break;
        case BM_CSR_MHPMCOUNTER10:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER10, mask);
            break;
        case BM_CSR_MHPMCOUNTER11:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER11, mask);
            break;
        case BM_CSR_MHPMCOUNTER12:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER12, mask);
            break;
        case BM_CSR_MHPMCOUNTER13:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER13, mask);
            break;
        case BM_CSR_MHPMCOUNTER14:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER14, mask);
            break;
        case BM_CSR_MHPMCOUNTER15:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER15, mask);
            break;
        case BM_CSR_MHPMCOUNTER16:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER16, mask);
            break;
        case BM_CSR_MHPMCOUNTER17:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER17, mask);
            break;
        case BM_CSR_MHPMCOUNTER18:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER18, mask);
            break;
        case BM_CSR_MHPMCOUNTER19:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER19, mask);
            break;
        case BM_CSR_MHPMCOUNTER20:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER20, mask);
            break;
        case BM_CSR_MHPMCOUNTER21:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER21, mask);
            break;
        case BM_CSR_MHPMCOUNTER22:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER22, mask);
            break;
        case BM_CSR_MHPMCOUNTER23:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER23, mask);
            break;
        case BM_CSR_MHPMCOUNTER24:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER24, mask);
            break;
        case BM_CSR_MHPMCOUNTER25:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER25, mask);
            break;
        case BM_CSR_MHPMCOUNTER26:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER26, mask);
            break;
        case BM_CSR_MHPMCOUNTER27:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER27, mask);
            break;
        case BM_CSR_MHPMCOUNTER28:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER28, mask);
            break;
        case BM_CSR_MHPMCOUNTER29:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER29, mask);
            break;
        case BM_CSR_MHPMCOUNTER30:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER30, mask);
            break;
        case BM_CSR_MHPMCOUNTER31:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER31, mask);
            break;
#endif
#if __riscv_xlen == 32
        case BM_CSR_MCYCLEH:
            CSR_CLEAR(BM_CSR_MCYCLEH, mask);
            break;
        case BM_CSR_MINSTRETH:
            CSR_CLEAR(BM_CSR_MINSTRETH, mask);
            break;
    #ifdef TARGET_HAS_HPM
        case BM_CSR_MHPMCOUNTER3H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER3H, mask);
            break;
        case BM_CSR_MHPMCOUNTER4H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER4H, mask);
            break;
        case BM_CSR_MHPMCOUNTER5H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER5H, mask);
            break;
        case BM_CSR_MHPMCOUNTER6H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER6H, mask);
            break;
        case BM_CSR_MHPMCOUNTER7H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER7H, mask);
            break;
        case BM_CSR_MHPMCOUNTER8H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER8H, mask);
            break;
        case BM_CSR_MHPMCOUNTER9H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER9H, mask);
            break;
        case BM_CSR_MHPMCOUNTER10H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER10H, mask);
            break;
        case BM_CSR_MHPMCOUNTER11H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER11H, mask);
            break;
        case BM_CSR_MHPMCOUNTER12H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER12H, mask);
            break;
        case BM_CSR_MHPMCOUNTER13H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER13H, mask);
            break;
        case BM_CSR_MHPMCOUNTER14H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER14H, mask);
            break;
        case BM_CSR_MHPMCOUNTER15H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER15H, mask);
            break;
        case BM_CSR_MHPMCOUNTER16H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER16H, mask);
            break;
        case BM_CSR_MHPMCOUNTER17H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER17H, mask);
            break;
        case BM_CSR_MHPMCOUNTER18H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER18H, mask);
            break;
        case BM_CSR_MHPMCOUNTER19H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER19H, mask);
            break;
        case BM_CSR_MHPMCOUNTER20H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER20H, mask);
            break;
        case BM_CSR_MHPMCOUNTER21H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER21H, mask);
            break;
        case BM_CSR_MHPMCOUNTER22H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER22H, mask);
            break;
        case BM_CSR_MHPMCOUNTER23H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER23H, mask);
            break;
        case BM_CSR_MHPMCOUNTER24H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER24H, mask);
            break;
        case BM_CSR_MHPMCOUNTER25H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER25H, mask);
            break;
        case BM_CSR_MHPMCOUNTER26H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER26H, mask);
            break;
        case BM_CSR_MHPMCOUNTER27H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER27H, mask);
            break;
        case BM_CSR_MHPMCOUNTER28H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER28H, mask);
            break;
        case BM_CSR_MHPMCOUNTER29H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER29H, mask);
            break;
        case BM_CSR_MHPMCOUNTER30H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER30H, mask);
            break;
        case BM_CSR_MHPMCOUNTER31H:
            CSR_CLEAR(BM_CSR_MHPMCOUNTER31H, mask);
            break;
    #endif
#endif
        case BM_CSR_CYCLE:
            CSR_CLEAR(BM_CSR_CYCLE, mask);
            break;
        case BM_CSR_TIME:
            CSR_CLEAR(BM_CSR_TIME, mask);
            break;
        case BM_CSR_INSTRET:
            CSR_CLEAR(BM_CSR_INSTRET, mask);
            break;
#ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3:
            CSR_CLEAR(BM_CSR_HPMCOUNTER3, mask);
            break;
        case BM_CSR_HPMCOUNTER4:
            CSR_CLEAR(BM_CSR_HPMCOUNTER4, mask);
            break;
        case BM_CSR_HPMCOUNTER5:
            CSR_CLEAR(BM_CSR_HPMCOUNTER5, mask);
            break;
        case BM_CSR_HPMCOUNTER6:
            CSR_CLEAR(BM_CSR_HPMCOUNTER6, mask);
            break;
        case BM_CSR_HPMCOUNTER7:
            CSR_CLEAR(BM_CSR_HPMCOUNTER7, mask);
            break;
        case BM_CSR_HPMCOUNTER8:
            CSR_CLEAR(BM_CSR_HPMCOUNTER8, mask);
            break;
        case BM_CSR_HPMCOUNTER9:
            CSR_CLEAR(BM_CSR_HPMCOUNTER9, mask);
            break;
        case BM_CSR_HPMCOUNTER10:
            CSR_CLEAR(BM_CSR_HPMCOUNTER10, mask);
            break;
        case BM_CSR_HPMCOUNTER11:
            CSR_CLEAR(BM_CSR_HPMCOUNTER11, mask);
            break;
        case BM_CSR_HPMCOUNTER12:
            CSR_CLEAR(BM_CSR_HPMCOUNTER12, mask);
            break;
        case BM_CSR_HPMCOUNTER13:
            CSR_CLEAR(BM_CSR_HPMCOUNTER13, mask);
            break;
        case BM_CSR_HPMCOUNTER14:
            CSR_CLEAR(BM_CSR_HPMCOUNTER14, mask);
            break;
        case BM_CSR_HPMCOUNTER15:
            CSR_CLEAR(BM_CSR_HPMCOUNTER15, mask);
            break;
        case BM_CSR_HPMCOUNTER16:
            CSR_CLEAR(BM_CSR_HPMCOUNTER16, mask);
            break;
        case BM_CSR_HPMCOUNTER17:
            CSR_CLEAR(BM_CSR_HPMCOUNTER17, mask);
            break;
        case BM_CSR_HPMCOUNTER18:
            CSR_CLEAR(BM_CSR_HPMCOUNTER18, mask);
            break;
        case BM_CSR_HPMCOUNTER19:
            CSR_CLEAR(BM_CSR_HPMCOUNTER19, mask);
            break;
        case BM_CSR_HPMCOUNTER20:
            CSR_CLEAR(BM_CSR_HPMCOUNTER20, mask);
            break;
        case BM_CSR_HPMCOUNTER21:
            CSR_CLEAR(BM_CSR_HPMCOUNTER21, mask);
            break;
        case BM_CSR_HPMCOUNTER22:
            CSR_CLEAR(BM_CSR_HPMCOUNTER22, mask);
            break;
        case BM_CSR_HPMCOUNTER23:
            CSR_CLEAR(BM_CSR_HPMCOUNTER23, mask);
            break;
        case BM_CSR_HPMCOUNTER24:
            CSR_CLEAR(BM_CSR_HPMCOUNTER24, mask);
            break;
        case BM_CSR_HPMCOUNTER25:
            CSR_CLEAR(BM_CSR_HPMCOUNTER25, mask);
            break;
        case BM_CSR_HPMCOUNTER26:
            CSR_CLEAR(BM_CSR_HPMCOUNTER26, mask);
            break;
        case BM_CSR_HPMCOUNTER27:
            CSR_CLEAR(BM_CSR_HPMCOUNTER27, mask);
            break;
        case BM_CSR_HPMCOUNTER28:
            CSR_CLEAR(BM_CSR_HPMCOUNTER28, mask);
            break;
        case BM_CSR_HPMCOUNTER29:
            CSR_CLEAR(BM_CSR_HPMCOUNTER29, mask);
            break;
        case BM_CSR_HPMCOUNTER30:
            CSR_CLEAR(BM_CSR_HPMCOUNTER30, mask);
            break;
        case BM_CSR_HPMCOUNTER31:
            CSR_CLEAR(BM_CSR_HPMCOUNTER31, mask);
            break;
#endif
#if __riscv_xlen == 32
        case BM_CSR_CYCLEH:
            CSR_CLEAR(BM_CSR_CYCLEH, mask);
            break;
        case BM_CSR_TIMEH:
            CSR_CLEAR(BM_CSR_TIMEH, mask);
            break;
        case BM_CSR_INSTRETH:
            CSR_CLEAR(BM_CSR_INSTRETH, mask);
            break;
    #ifdef TARGET_HAS_HPM
        case BM_CSR_HPMCOUNTER3H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER3H, mask);
            break;
        case BM_CSR_HPMCOUNTER4H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER4H, mask);
            break;
        case BM_CSR_HPMCOUNTER5H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER5H, mask);
            break;
        case BM_CSR_HPMCOUNTER6H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER6H, mask);
            break;
        case BM_CSR_HPMCOUNTER7H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER7H, mask);
            break;
        case BM_CSR_HPMCOUNTER8H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER8H, mask);
            break;
        case BM_CSR_HPMCOUNTER9H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER9H, mask);
            break;
        case BM_CSR_HPMCOUNTER10H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER10H, mask);
            break;
        case BM_CSR_HPMCOUNTER11H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER11H, mask);
            break;
        case BM_CSR_HPMCOUNTER12H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER12H, mask);
            break;
        case BM_CSR_HPMCOUNTER13H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER13H, mask);
            break;
        case BM_CSR_HPMCOUNTER14H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER14H, mask);
            break;
        case BM_CSR_HPMCOUNTER15H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER15H, mask);
            break;
        case BM_CSR_HPMCOUNTER16H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER16H, mask);
            break;
        case BM_CSR_HPMCOUNTER17H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER17H, mask);
            break;
        case BM_CSR_HPMCOUNTER18H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER18H, mask);
            break;
        case BM_CSR_HPMCOUNTER19H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER19H, mask);
            break;
        case BM_CSR_HPMCOUNTER20H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER20H, mask);
            break;
        case BM_CSR_HPMCOUNTER21H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER21H, mask);
            break;
        case BM_CSR_HPMCOUNTER22H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER22H, mask);
            break;
        case BM_CSR_HPMCOUNTER23H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER23H, mask);
            break;
        case BM_CSR_HPMCOUNTER24H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER24H, mask);
            break;
        case BM_CSR_HPMCOUNTER25H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER25H, mask);
            break;
        case BM_CSR_HPMCOUNTER26H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER26H, mask);
            break;
        case BM_CSR_HPMCOUNTER27H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER27H, mask);
            break;
        case BM_CSR_HPMCOUNTER28H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER28H, mask);
            break;
        case BM_CSR_HPMCOUNTER29H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER29H, mask);
            break;
        case BM_CSR_HPMCOUNTER30H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER30H, mask);
            break;
        case BM_CSR_HPMCOUNTER31H:
            CSR_CLEAR(BM_CSR_HPMCOUNTER31H, mask);
            break;
    #endif
#endif
        case BM_CSR_MVENDORID:
            CSR_CLEAR(BM_CSR_MVENDORID, mask);
            break;
        case BM_CSR_MARCHID:
            CSR_CLEAR(BM_CSR_MARCHID, mask);
            break;
        case BM_CSR_MIMPID:
            CSR_CLEAR(BM_CSR_MIMPID, mask);
            break;
        case BM_CSR_MHARTID:
            CSR_CLEAR(BM_CSR_MHARTID, mask);
            break;
        default:
#ifdef TARGET_HAS_CUSTOM_CSR
            target_csr_clear_mask(csr, mask);
#else
            bm_error("Unsupported CSR.");
#endif
            break;
    }
}

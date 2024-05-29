/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/priv.h"

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/interrupt_low.h"
#include "baremetal/verbose.h"

// Current privilege level
volatile bm_priv_mode_t bm_current_mode = BM_PRIV_MODE_MACHINE;

// Stack pointers saved for each privilege level,
// stack pointer only needs to be saved once lower privilege mode is entered
volatile xlen_t bm_priv_sp[4] = {0};

// Registers from execution in previous privilege mode saved upon trapping
// to highed privilege mode on interrupt/exception
volatile bm_register_file_t bm_priv_regs[4] = {{0}};

bm_priv_mode_t bm_get_priv_mode()
{
    return bm_current_mode;
}

bm_csr_id bm_priv_get_csr_id(bm_priv_mode_t priv_mode, bm_csr_type_t csr_type)
{
    switch (priv_mode)
    {
        case BM_PRIV_MODE_MACHINE:
            switch (csr_type)
            {
                case BM_PRIV_CSR_XCAUSE:
                    return BM_CSR_MCAUSE;
                case BM_PRIV_CSR_XEPC:
                    return BM_CSR_MEPC;
                case BM_PRIV_CSR_XIE:
                    return BM_CSR_MIE;
                case BM_PRIV_CSR_XIP:
                    return BM_CSR_MIP;
                case BM_PRIV_CSR_XSTATUS:
                    return BM_CSR_MSTATUS;
                case BM_PRIV_CSR_XTVAL:
                    return BM_CSR_MTVAL;
                case BM_PRIV_CSR_XTVEC:
                    return BM_CSR_MTVEC;
                default:
                    bm_error("Unknown csr type.");
            }
#ifdef TARGET_EXT_S
        case BM_PRIV_MODE_SUPERVISOR:
            switch (csr_type)
            {
                case BM_PRIV_CSR_XCAUSE:
                    return BM_CSR_SCAUSE;
                case BM_PRIV_CSR_XEPC:
                    return BM_CSR_SEPC;
                case BM_PRIV_CSR_XIE:
                    return BM_CSR_SIE;
                case BM_PRIV_CSR_XIP:
                    return BM_CSR_SIP;
                case BM_PRIV_CSR_XSTATUS:
                    return BM_CSR_SSTATUS;
                case BM_PRIV_CSR_XTVAL:
                    return BM_CSR_STVAL;
                case BM_PRIV_CSR_XTVEC:
                    return BM_CSR_STVEC;
                default:
                    bm_error("Unknown csr type.");
            }
#endif
#ifdef TARGET_EXT_N
        case BM_PRIV_MODE_USER:
            switch (csr_type)
            {
                case BM_PRIV_CSR_XCAUSE:
                    return BM_CSR_UCAUSE;
                case BM_PRIV_CSR_XEPC:
                    return BM_CSR_UEPC;
                case BM_PRIV_CSR_XIE:
                    return BM_CSR_UIE;
                case BM_PRIV_CSR_XIP:
                    return BM_CSR_UIP;
                case BM_PRIV_CSR_XSTATUS:
                    return BM_CSR_USTATUS;
                case BM_PRIV_CSR_XTVAL:
                    return BM_CSR_UTVAL;
                case BM_PRIV_CSR_XTVEC:
                    return BM_CSR_UTVEC;
                default:
                    bm_error("Unknown csr type.");
            }
#endif
        default:
            bm_error("Unsupported privilege mode.");
    }
}

/**
 * \brief Helper called in case of unexpected return from lower privilege mode entry function
 */
static void bm_priv_enter_mode_error(void)
{
    bm_error("Return from a privilege entry function detected.");
}

// Previous privilege mode is stored in [m,s]status CSR
#define BM_MSTATUS_MPP_OFFSET 11
#define BM_MSTATUS_MPP_MASK   0x3
#define BM_MSTATUS_SPP_OFFSET 8
#define BM_MSTATUS_SPP_MASK   0x1

void __attribute__((noreturn)) bm_priv_enter_mode(bm_priv_mode_t mode, xlen_t entry, xlen_t stack)
{
    if (mode >= bm_current_mode)
    {
        bm_error("Only dropping privilege to lover level is possible.");
    }

    // Save stack pointer for the current privilege mode
    __asm__ volatile("mv %0, sp\n" : "=r"(bm_priv_sp[bm_current_mode]));

    // Update internal variable holding privilege mode
    bm_priv_mode_t prev_mode = bm_current_mode;
    bm_current_mode          = mode;

    switch (prev_mode)
    {
        case BM_PRIV_MODE_MACHINE:
            bm_csr_clear_mask(BM_CSR_MSTATUS, BM_MSTATUS_MPP_MASK << BM_MSTATUS_MPP_OFFSET);
            bm_csr_set_mask(BM_CSR_MSTATUS, mode << BM_MSTATUS_MPP_OFFSET);
            bm_csr_write(BM_CSR_MEPC, (xlen_t)entry);

            __asm__ volatile("la ra, %0\n"
                             "mv sp, %1\n"
                             "mret" ::"i"(bm_priv_enter_mode_error),
                             "r"(stack));
            break;
#ifdef TARGET_EXT_S
        case BM_PRIV_MODE_SUPERVISOR:
            bm_csr_clear_mask(BM_CSR_SSTATUS, BM_MSTATUS_SPP_MASK << BM_MSTATUS_SPP_OFFSET);
            bm_csr_set_mask(BM_CSR_SSTATUS, mode << BM_MSTATUS_SPP_OFFSET);
            bm_csr_write(BM_CSR_SEPC, (xlen_t)entry);

            __asm__ volatile("la ra, %0\n"
                             "mv sp, %1\n"
                             "sret" ::"i"(bm_priv_enter_mode_error),
                             "r"(stack));
            break;
#endif
        default:
            bm_error("Unsupported privilege mode.");
    }

    // should not get here
    exit(1);
}

xlen_t bm_priv_execute_call(bm_priv_call_args args)
{
    xlen_t ret;
    __asm__ volatile("mv a0, %1\n"
                     "mv a1, %2\n"
                     "mv a2, %3\n"
                     "mv a3, %4\n"
                     "mv a4, %5\n"
                     "mv a5, %6\n"
                     "mv a6, %7\n"
                     "mv a7, %8\n"
                     "ecall\n"
                     "mv %0, a0"
                     : "=r"(ret)
                     : "r"(args.arg0),
                       "r"(args.arg1),
                       "r"(args.arg2),
                       "r"(args.arg3),
                       "r"(args.arg4),
                       "r"(args.arg5),
                       "r"(args.arg6),
                       "r"(args.arg7)
                     : "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7");
    return ret;
}

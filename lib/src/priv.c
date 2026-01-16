/* Copyright 2023-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/priv.h"

#include "baremetal/bm_cheri.h"
#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/interrupt_low.h"
#include "baremetal/mp.h"
#include "baremetal/verbose.h"

// Current privilege level
volatile bm_priv_mode_t bm_current_mode = BM_PRIV_MODE_MACHINE;

bm_priv_mode_t bm_get_priv_mode(void)
{
    return bm_current_mode;
}

bm_csr_id_t bm_priv_get_csr_id(bm_priv_mode_t priv_mode, bm_csr_type_t csr_type)
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
                    bm_fatal("No M-mode CSR for type %d.", csr_type);
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
                    bm_fatal("No S-mode CSR for type %d.", csr_type);
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
                    bm_fatal("No U-mode CSR for type %d.", csr_type);
            }
#endif
        default:
            bm_fatal("Unsupported privilege mode %d.", priv_mode);
    }
}

xlen_t bm_priv_csr_read(bm_priv_mode_t priv_mode, bm_csr_type_t csr_type)
{
    bm_csr_id_t csr = bm_priv_get_csr_id(priv_mode, csr_type);
    xlen_t      val = 0;
#ifdef __CHERI_PURE_CAPABILITY__
    void *ptr;
#endif

    switch (csr)
    {
        case BM_CSR_MCAUSE:
            BM_CSR_READ(BM_CSR_MCAUSE, val);
            break;
        case BM_CSR_MEPC:
#ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_READ_CAP(mepcc, ptr);
            val = (xlen_t)ptr;

#else
            BM_CSR_READ(BM_CSR_MEPC, val);
#endif
            break;
        case BM_CSR_MIE:
            BM_CSR_READ(BM_CSR_MIE, val);
            break;
        case BM_CSR_MIP:
            BM_CSR_READ(BM_CSR_MIP, val);
            break;
        case BM_CSR_MSTATUS:
            BM_CSR_READ(BM_CSR_MSTATUS, val);
            break;
        case BM_CSR_MTVAL:
            BM_CSR_READ(BM_CSR_MTVAL, val);
            break;
        case BM_CSR_MTVEC:
#ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_READ_CAP(mtvecc, ptr);
            val = (xlen_t)ptr;

#else
            BM_CSR_READ(BM_CSR_MTVEC, val);
#endif
            break;
#ifdef TARGET_EXT_S
        case BM_CSR_SCAUSE:
            BM_CSR_READ(BM_CSR_SCAUSE, val);
            break;
        case BM_CSR_SEPC:
    #ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_READ_CAP(sepcc, ptr);
            val = (xlen_t)ptr;

    #else
            BM_CSR_READ(BM_CSR_SEPC, val);
    #endif
            break;
        case BM_CSR_SIE:
            BM_CSR_READ(BM_CSR_SIE, val);
            break;
        case BM_CSR_SIP:
            BM_CSR_READ(BM_CSR_SIP, val);
            break;
        case BM_CSR_SSTATUS:
            BM_CSR_READ(BM_CSR_SSTATUS, val);
            break;
        case BM_CSR_STVAL:
            BM_CSR_READ(BM_CSR_STVAL, val);
            break;
        case BM_CSR_STVEC:
    #ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_READ_CAP(stvecc, ptr);
            val = (xlen_t)ptr;

    #else
            BM_CSR_READ(BM_CSR_STVEC, val);
    #endif
            break;
#endif // TARGET_EXT_S
#ifdef TARGET_EXT_N
        case BM_CSR_UCAUSE:
            BM_CSR_READ(BM_CSR_UCAUSE, val);
            break;
        case BM_CSR_UEPC:
    #ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_READ_CAP(uepcc, ptr);
            val = (xlen_t)ptr;

    #else
            BM_CSR_READ(BM_CSR_UEPC, val);
    #endif
            break;
        case BM_CSR_UIE:
            BM_CSR_READ(BM_CSR_UIE, val);
            break;
        case BM_CSR_UIP:
            BM_CSR_READ(BM_CSR_UIP, val);
            break;
        case BM_CSR_USTATUS:
            BM_CSR_READ(BM_CSR_USTATUS, val);
            break;
        case BM_CSR_UTVAL:
            BM_CSR_READ(BM_CSR_UTVAL, val);
            break;
        case BM_CSR_UTVEC:
            BM_CSR_READ(BM_CSR_UTVEC, val);
            break;
#endif // TARGET_EXT_N
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }

    return val;
}

void bm_priv_csr_write(bm_priv_mode_t priv_mode, bm_csr_type_t csr_type, xlen_t val)
{
    bm_csr_id_t csr = bm_priv_get_csr_id(priv_mode, csr_type);
    switch (csr)
    {
        case BM_CSR_MCAUSE:
            BM_CSR_WRITE(BM_CSR_MCAUSE, val);
            break;
        case BM_CSR_MEPC:
#ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_WRITE_CAP(mepcc, addr_to_code_ptr(val));

#else
            BM_CSR_WRITE(BM_CSR_MEPC, val);
#endif
            break;
        case BM_CSR_MIE:
            BM_CSR_WRITE(BM_CSR_MIE, val);
            break;
        case BM_CSR_MIP:
            BM_CSR_WRITE(BM_CSR_MIP, val);
            break;
        case BM_CSR_MSTATUS:
            BM_CSR_WRITE(BM_CSR_MSTATUS, val);
            break;
        case BM_CSR_MTVAL:
            BM_CSR_WRITE(BM_CSR_MTVAL, val);
            break;
        case BM_CSR_MTVEC:
#ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_WRITE_CAP(mtvecc, addr_to_code_ptr(val));

#else
            BM_CSR_WRITE(BM_CSR_MTVEC, val);
#endif
            break;
#ifdef TARGET_EXT_S
        case BM_CSR_SCAUSE:
            BM_CSR_WRITE(BM_CSR_SCAUSE, val);
            break;
        case BM_CSR_SEPC:
    #ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_WRITE_CAP(sepcc, addr_to_code_ptr(val));

    #else
            BM_CSR_WRITE(BM_CSR_SEPC, val);
    #endif
            break;
        case BM_CSR_SIE:
            BM_CSR_WRITE(BM_CSR_SIE, val);
            break;
        case BM_CSR_SIP:
            BM_CSR_WRITE(BM_CSR_SIP, val);
            break;
        case BM_CSR_SSTATUS:
            BM_CSR_WRITE(BM_CSR_SSTATUS, val);
            break;
        case BM_CSR_STVAL:
            BM_CSR_WRITE(BM_CSR_STVAL, val);
            break;
        case BM_CSR_STVEC:
    #ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_WRITE_CAP(stvecc, addr_to_code_ptr(val));

    #else
            BM_CSR_WRITE(BM_CSR_STVEC, val);
    #endif
            break;
#endif // TARGET_EXT_S
#ifdef TARGET_EXT_N
        case BM_CSR_UCAUSE:
            BM_CSR_WRITE(BM_CSR_UCAUSE, val);
            break;
        case BM_CSR_UEPC:
    #ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_WRITE_CAP(uepcc, addr_to_code_ptr(val));

    #else
            BM_CSR_WRITE(BM_CSR_UEPC, val);
    #endif
            break;
        case BM_CSR_UIE:
            BM_CSR_WRITE(BM_CSR_UIE, val);
            break;
        case BM_CSR_UIP:
            BM_CSR_WRITE(BM_CSR_UIP, val);
            break;
        case BM_CSR_USTATUS:
            BM_CSR_WRITE(BM_CSR_USTATUS, val);
            break;
        case BM_CSR_UTVAL:
            BM_CSR_WRITE(BM_CSR_UTVAL, val);
            break;
        case BM_CSR_UTVEC:
            BM_CSR_WRITE(BM_CSR_UTVEC, val);
            break;
#endif // TARGET_EXT_N
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }
}

void bm_priv_csr_set(bm_priv_mode_t priv_mode, bm_csr_type_t csr_type, xlen_t mask)
{
    bm_csr_id_t csr = bm_priv_get_csr_id(priv_mode, csr_type);
    switch (csr)
    {
        case BM_CSR_MCAUSE:
            BM_CSR_SET(BM_CSR_MCAUSE, mask);
            break;
        case BM_CSR_MEPC:
            BM_CSR_SET(BM_CSR_MEPC, mask);
            break;
        case BM_CSR_MIE:
            BM_CSR_SET(BM_CSR_MIE, mask);
            break;
        case BM_CSR_MIP:
            BM_CSR_SET(BM_CSR_MIP, mask);
            break;
        case BM_CSR_MSTATUS:
            BM_CSR_SET(BM_CSR_MSTATUS, mask);
            break;
        case BM_CSR_MTVAL:
            BM_CSR_SET(BM_CSR_MTVAL, mask);
            break;
        case BM_CSR_MTVEC:
            BM_CSR_SET(BM_CSR_MTVEC, mask);
            break;
#ifdef TARGET_EXT_S
        case BM_CSR_SCAUSE:
            BM_CSR_SET(BM_CSR_SCAUSE, mask);
            break;
        case BM_CSR_SEPC:
            BM_CSR_SET(BM_CSR_SEPC, mask);
            break;
        case BM_CSR_SIE:
            BM_CSR_SET(BM_CSR_SIE, mask);
            break;
        case BM_CSR_SIP:
            BM_CSR_SET(BM_CSR_SIP, mask);
            break;
        case BM_CSR_SSTATUS:
            BM_CSR_SET(BM_CSR_SSTATUS, mask);
            break;
        case BM_CSR_STVAL:
            BM_CSR_SET(BM_CSR_STVAL, mask);
            break;
        case BM_CSR_STVEC:
            BM_CSR_SET(BM_CSR_STVEC, mask);
            break;
#endif // TARGET_EXT_S
#ifdef TARGET_EXT_N
        case BM_CSR_UCAUSE:
            BM_CSR_SET(BM_CSR_UCAUSE, mask);
            break;
        case BM_CSR_UEPC:
            BM_CSR_SET(BM_CSR_UEPC, mask);
            break;
        case BM_CSR_UIE:
            BM_CSR_SET(BM_CSR_UIE, mask);
            break;
        case BM_CSR_UIP:
            BM_CSR_SET(BM_CSR_UIP, mask);
            break;
        case BM_CSR_USTATUS:
            BM_CSR_SET(BM_CSR_USTATUS, mask);
            break;
        case BM_CSR_UTVAL:
            BM_CSR_SET(BM_CSR_UTVAL, mask);
            break;
        case BM_CSR_UTVEC:
            BM_CSR_SET(BM_CSR_UTVEC, mask);
            break;
#endif // TARGET_EXT_N
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }
}

void bm_priv_csr_clear(bm_priv_mode_t priv_mode, bm_csr_type_t csr_type, xlen_t mask)
{
    bm_csr_id_t csr = bm_priv_get_csr_id(priv_mode, csr_type);
    switch (csr)
    {
        case BM_CSR_MCAUSE:
            BM_CSR_CLEAR(BM_CSR_MCAUSE, mask);
            break;
        case BM_CSR_MEPC:
            BM_CSR_CLEAR(BM_CSR_MEPC, mask);
            break;
        case BM_CSR_MIE:
            BM_CSR_CLEAR(BM_CSR_MIE, mask);
            break;
        case BM_CSR_MIP:
            BM_CSR_CLEAR(BM_CSR_MIP, mask);
            break;
        case BM_CSR_MSTATUS:
            BM_CSR_CLEAR(BM_CSR_MSTATUS, mask);
            break;
        case BM_CSR_MTVAL:
            BM_CSR_CLEAR(BM_CSR_MTVAL, mask);
            break;
        case BM_CSR_MTVEC:
            BM_CSR_CLEAR(BM_CSR_MTVEC, mask);
            break;
#ifdef TARGET_EXT_S
        case BM_CSR_SCAUSE:
            BM_CSR_CLEAR(BM_CSR_SCAUSE, mask);
            break;
        case BM_CSR_SEPC:
            BM_CSR_CLEAR(BM_CSR_SEPC, mask);
            break;
        case BM_CSR_SIE:
            BM_CSR_CLEAR(BM_CSR_SIE, mask);
            break;
        case BM_CSR_SIP:
            BM_CSR_CLEAR(BM_CSR_SIP, mask);
            break;
        case BM_CSR_SSTATUS:
            BM_CSR_CLEAR(BM_CSR_SSTATUS, mask);
            break;
        case BM_CSR_STVAL:
            BM_CSR_CLEAR(BM_CSR_STVAL, mask);
            break;
        case BM_CSR_STVEC:
            BM_CSR_CLEAR(BM_CSR_STVEC, mask);
            break;
#endif // TARGET_EXT_S
#ifdef TARGET_EXT_N
        case BM_CSR_UCAUSE:
            BM_CSR_CLEAR(BM_CSR_UCAUSE, mask);
            break;
        case BM_CSR_UEPC:
            BM_CSR_CLEAR(BM_CSR_UEPC, mask);
            break;
        case BM_CSR_UIE:
            BM_CSR_CLEAR(BM_CSR_UIE, mask);
            break;
        case BM_CSR_UIP:
            BM_CSR_CLEAR(BM_CSR_UIP, mask);
            break;
        case BM_CSR_USTATUS:
            BM_CSR_CLEAR(BM_CSR_USTATUS, mask);
            break;
        case BM_CSR_UTVAL:
            BM_CSR_CLEAR(BM_CSR_UTVAL, mask);
            break;
        case BM_CSR_UTVEC:
            BM_CSR_CLEAR(BM_CSR_UTVEC, mask);
            break;
#endif // TARGET_EXT_N
        default:
            bm_fatal("Unsupported CSR 0x%x", csr);
            break;
    }
}

/**
 * \brief Helper called in case of unexpected return from lower privilege mode entry function
 */
static void bm_priv_enter_mode_error(void)
{
    bm_fatal("Return from a privilege entry function detected.");
}

// Previous privilege mode is stored in [m,s]status CSR
#define BM_MSTATUS_MPP_OFFSET 11
#define BM_MSTATUS_MPP_MASK   0x3
#define BM_MSTATUS_SPP_OFFSET 8
#define BM_MSTATUS_SPP_MASK   0x1

void __attribute__((noreturn)) bm_priv_enter_mode(bm_priv_mode_t mode, entry_fn_t entry, uint8_t *stack)
{
    if (mode >= bm_current_mode)
    {
        bm_fatal("Only dropping privilege to lower level is possible.");
    }

    // Update internal variable holding privilege mode
    bm_priv_mode_t prev_mode = bm_current_mode;
    bm_current_mode          = mode;

    switch (prev_mode)
    {
        case BM_PRIV_MODE_MACHINE:
            BM_CSR_CLEAR(BM_CSR_MSTATUS, BM_MSTATUS_MPP_MASK << BM_MSTATUS_MPP_OFFSET);
            BM_CSR_SET(BM_CSR_MSTATUS, mode << BM_MSTATUS_MPP_OFFSET);

#ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_WRITE_CAP(mepcc, entry);

            __asm__ volatile("mv cra, %0\n"
                             "mv csp, %1\n"
                             /* Enable Cheri in Supervisor and User modes */
                             "li t0, 1 << 28\n"   /* Bit 28 is the Y (Cheri) Enable bit */
                             "csrs menvcfg, t0\n" /* menvcfg.Y = Enabled */
                             "csrs senvcfg, t0\n" /* senvcfg.Y = Enabled */
                             "mret" ::"C"(bm_priv_enter_mode_error),
                             "C"(stack));

#else
            BM_CSR_WRITE(BM_CSR_MEPC, (xlen_t)entry);

            __asm__ volatile("la ra, %0\n"
                             "mv sp, %1\n"
                             "mret" ::"i"(bm_priv_enter_mode_error),
                             "r"(stack));
#endif
            break;
#ifdef TARGET_EXT_S
        case BM_PRIV_MODE_SUPERVISOR:
            BM_CSR_CLEAR(BM_CSR_SSTATUS, BM_MSTATUS_SPP_MASK << BM_MSTATUS_SPP_OFFSET);
            BM_CSR_SET(BM_CSR_SSTATUS, mode << BM_MSTATUS_SPP_OFFSET);

    #ifdef __CHERI_PURE_CAPABILITY__
            BM_CSR_WRITE_CAP(sepcc, entry);

            __asm__ volatile("mv cra, %0\n"
                             "mv csp, %1\n"
                             "sret" ::"C"(bm_priv_enter_mode_error),
                             "C"(stack));

    #else
            BM_CSR_WRITE(BM_CSR_SEPC, (xlen_t)entry);

            __asm__ volatile("la ra, %0\n"
                             "mv sp, %1\n"
                             "sret" ::"i"(bm_priv_enter_mode_error),
                             "r"(stack));
    #endif
            break;
#endif
        default:
            bm_fatal("Unsupported privilege mode.");
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

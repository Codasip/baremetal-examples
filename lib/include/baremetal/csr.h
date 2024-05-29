/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_CSR_H
#define BAREMETAL_CSR_H

#include "baremetal/common.h"

#define CSR_READ(csr, value)  __asm__ volatile("csrr %0, %1" : "=r"(value) : "i"(csr))
#define CSR_WRITE(csr, value) __asm__ volatile("csrw %0, %1" : : "i"(csr), "r"(value))
#define CSR_SET(csr, mask)    __asm__ volatile("csrs %0, %1" : : "i"(csr), "r"(mask))
#define CSR_CLEAR(csr, mask)  __asm__ volatile("csrc %0, %1" : : "i"(csr), "r"(mask))

#ifdef __cplusplus
extern "C" {
#endif

#define BM_CSR_UNDEFINED -1

/** \brief Enumeration of standard control and status registers available on the target */
typedef enum {
    BM_CSR_USTATUS        = 0x000,
    BM_CSR_FFLAGS         = 0x001,
    BM_CSR_FRM            = 0x002,
    BM_CSR_FCSR           = 0x003,
    BM_CSR_UIE            = 0x004,
    BM_CSR_UTVEC          = 0x005,
    BM_CSR_SEED           = 0x015,
    BM_CSR_USCRATCH       = 0x040,
    BM_CSR_UEPC           = 0x041,
    BM_CSR_UCAUSE         = 0x042,
    BM_CSR_UTVAL          = 0x043,
    BM_CSR_UIP            = 0x044,
    BM_CSR_SSTATUS        = 0x100,
    BM_CSR_SEDELEG        = 0x102,
    BM_CSR_SIDELEG        = 0x103,
    BM_CSR_SIE            = 0x104,
    BM_CSR_STVEC          = 0x105,
    BM_CSR_SCOUNTEREN     = 0x106,
    BM_CSR_STVT           = 0x107,
    BM_CSR_SENVCFG        = 0x10a,
    BM_CSR_SSCRATCH       = 0x140,
    BM_CSR_SEPC           = 0x141,
    BM_CSR_SCAUSE         = 0x142,
    BM_CSR_STVAL          = 0x143,
    BM_CSR_SIP            = 0x144,
    BM_CSR_SNXTI          = 0x145,
    BM_CSR_SINTTHRESH     = 0x147,
    BM_CSR_SSCRATCHCSW    = 0x148,
    BM_CSR_SSCRATCHCSWL   = 0x149,
    BM_CSR_SATP           = 0x180,
    BM_CSR_MSTATUS        = 0x300,
    BM_CSR_MISA           = 0x301,
    BM_CSR_MEDELEG        = 0x302,
    BM_CSR_MIDELEG        = 0x303,
    BM_CSR_MIE            = 0x304,
    BM_CSR_MTVEC          = 0x305,
    BM_CSR_MCOUNTEREN     = 0x306,
    BM_CSR_MTVT           = 0x307,
    BM_CSR_MENVCFG        = 0x30a,
    BM_CSR_MENVCFGH       = 0x31a,
    BM_CSR_MCOUNTINHIBIT  = 0x320,
    BM_CSR_MHPMEVENT3     = 0x323,
    BM_CSR_MHPMEVENT4     = 0x324,
    BM_CSR_MHPMEVENT5     = 0x325,
    BM_CSR_MHPMEVENT6     = 0x326,
    BM_CSR_MHPMEVENT7     = 0x327,
    BM_CSR_MHPMEVENT8     = 0x328,
    BM_CSR_MHPMEVENT9     = 0x329,
    BM_CSR_MHPMEVENT10    = 0x32a,
    BM_CSR_MHPMEVENT11    = 0x32b,
    BM_CSR_MHPMEVENT12    = 0x32c,
    BM_CSR_MHPMEVENT13    = 0x32d,
    BM_CSR_MHPMEVENT14    = 0x32e,
    BM_CSR_MHPMEVENT15    = 0x32f,
    BM_CSR_MHPMEVENT16    = 0x330,
    BM_CSR_MHPMEVENT17    = 0x331,
    BM_CSR_MHPMEVENT18    = 0x332,
    BM_CSR_MHPMEVENT19    = 0x333,
    BM_CSR_MHPMEVENT20    = 0x334,
    BM_CSR_MHPMEVENT21    = 0x335,
    BM_CSR_MHPMEVENT22    = 0x336,
    BM_CSR_MHPMEVENT23    = 0x337,
    BM_CSR_MHPMEVENT24    = 0x338,
    BM_CSR_MHPMEVENT25    = 0x339,
    BM_CSR_MHPMEVENT26    = 0x33a,
    BM_CSR_MHPMEVENT27    = 0x33b,
    BM_CSR_MHPMEVENT28    = 0x33c,
    BM_CSR_MHPMEVENT29    = 0x33d,
    BM_CSR_MHPMEVENT30    = 0x33e,
    BM_CSR_MHPMEVENT31    = 0x33f,
    BM_CSR_MSCRATCH       = 0x340,
    BM_CSR_MEPC           = 0x341,
    BM_CSR_MCAUSE         = 0x342,
    BM_CSR_MTVAL          = 0x343,
    BM_CSR_MIP            = 0x344,
    BM_CSR_MNXTI          = 0x345,
    BM_CSR_MINTTHRESH     = 0x347,
    BM_CSR_MSCRATCHCSW    = 0x348,
    BM_CSR_MSCRATCHCSWL   = 0x349,
    BM_CSR_PMPCFG0        = 0x3a0,
    BM_CSR_PMPCFG1        = 0x3a1,
    BM_CSR_PMPCFG2        = 0x3a2,
    BM_CSR_PMPCFG3        = 0x3a3,
    BM_CSR_PMPADDR0       = 0x3b0,
    BM_CSR_PMPADDR1       = 0x3b1,
    BM_CSR_PMPADDR2       = 0x3b2,
    BM_CSR_PMPADDR3       = 0x3b3,
    BM_CSR_PMPADDR4       = 0x3b4,
    BM_CSR_PMPADDR5       = 0x3b5,
    BM_CSR_PMPADDR6       = 0x3b6,
    BM_CSR_PMPADDR7       = 0x3b7,
    BM_CSR_PMPADDR8       = 0x3b8,
    BM_CSR_PMPADDR9       = 0x3b9,
    BM_CSR_PMPADDR10      = 0x3ba,
    BM_CSR_PMPADDR11      = 0x3bb,
    BM_CSR_PMPADDR12      = 0x3bc,
    BM_CSR_PMPADDR13      = 0x3bd,
    BM_CSR_PMPADDR14      = 0x3be,
    BM_CSR_PMPADDR15      = 0x3bf,
    BM_CSR_MSECCFG        = 0x747,
    BM_CSR_MSECCFGH       = 0x757,
    BM_CSR_TSELECT        = 0x7a0,
    BM_CSR_TDATA1         = 0x7a1,
    BM_CSR_TDATA2         = 0x7a2,
    BM_CSR_TDATA3         = 0x7a3,
    BM_CSR_TINFO          = 0x7a4,
    BM_CSR_TCONTROL       = 0x7a5,
    BM_CSR_MCONTEXT       = 0x7a8,
    BM_CSR_MPICFLAG       = 0x7c1,
    BM_CSR_MPICMASK       = 0x7c2,
    BM_CSR_MPICFLAG1      = 0x7c4,
    BM_CSR_MPICMASK1      = 0x7c5,
    BM_CSR_MPICFLAG2      = 0x7c7,
    BM_CSR_MPICMASK2      = 0x7c8,
    BM_CSR_MPICFLAG3      = 0x7ca,
    BM_CSR_MPICMASK3      = 0x7cb,
    BM_CSR_MCYCLE         = 0xb00,
    BM_CSR_MINSTRET       = 0xb02,
    BM_CSR_MHPMCOUNTER3   = 0xb03,
    BM_CSR_MHPMCOUNTER4   = 0xb04,
    BM_CSR_MHPMCOUNTER5   = 0xb05,
    BM_CSR_MHPMCOUNTER6   = 0xb06,
    BM_CSR_MHPMCOUNTER7   = 0xb07,
    BM_CSR_MHPMCOUNTER8   = 0xb08,
    BM_CSR_MHPMCOUNTER9   = 0xb09,
    BM_CSR_MHPMCOUNTER10  = 0xb0a,
    BM_CSR_MHPMCOUNTER11  = 0xb0b,
    BM_CSR_MHPMCOUNTER12  = 0xb0c,
    BM_CSR_MHPMCOUNTER13  = 0xb0d,
    BM_CSR_MHPMCOUNTER14  = 0xb0e,
    BM_CSR_MHPMCOUNTER15  = 0xb0f,
    BM_CSR_MHPMCOUNTER16  = 0xb10,
    BM_CSR_MHPMCOUNTER17  = 0xb11,
    BM_CSR_MHPMCOUNTER18  = 0xb12,
    BM_CSR_MHPMCOUNTER19  = 0xb13,
    BM_CSR_MHPMCOUNTER20  = 0xb14,
    BM_CSR_MHPMCOUNTER21  = 0xb15,
    BM_CSR_MHPMCOUNTER22  = 0xb16,
    BM_CSR_MHPMCOUNTER23  = 0xb17,
    BM_CSR_MHPMCOUNTER24  = 0xb18,
    BM_CSR_MHPMCOUNTER25  = 0xb19,
    BM_CSR_MHPMCOUNTER26  = 0xb1a,
    BM_CSR_MHPMCOUNTER27  = 0xb1b,
    BM_CSR_MHPMCOUNTER28  = 0xb1c,
    BM_CSR_MHPMCOUNTER29  = 0xb1d,
    BM_CSR_MHPMCOUNTER30  = 0xb1e,
    BM_CSR_MHPMCOUNTER31  = 0xb1f,
    BM_CSR_MCYCLEH        = 0xb80,
    BM_CSR_MINSTRETH      = 0xb82,
    BM_CSR_MHPMCOUNTER3H  = 0xb83,
    BM_CSR_MHPMCOUNTER4H  = 0xb84,
    BM_CSR_MHPMCOUNTER5H  = 0xb85,
    BM_CSR_MHPMCOUNTER6H  = 0xb86,
    BM_CSR_MHPMCOUNTER7H  = 0xb87,
    BM_CSR_MHPMCOUNTER8H  = 0xb88,
    BM_CSR_MHPMCOUNTER9H  = 0xb89,
    BM_CSR_MHPMCOUNTER10H = 0xb8a,
    BM_CSR_MHPMCOUNTER11H = 0xb8b,
    BM_CSR_MHPMCOUNTER12H = 0xb8c,
    BM_CSR_MHPMCOUNTER13H = 0xb8d,
    BM_CSR_MHPMCOUNTER14H = 0xb8e,
    BM_CSR_MHPMCOUNTER15H = 0xb8f,
    BM_CSR_MHPMCOUNTER16H = 0xb90,
    BM_CSR_MHPMCOUNTER17H = 0xb91,
    BM_CSR_MHPMCOUNTER18H = 0xb92,
    BM_CSR_MHPMCOUNTER19H = 0xb93,
    BM_CSR_MHPMCOUNTER20H = 0xb94,
    BM_CSR_MHPMCOUNTER21H = 0xb95,
    BM_CSR_MHPMCOUNTER22H = 0xb96,
    BM_CSR_MHPMCOUNTER23H = 0xb97,
    BM_CSR_MHPMCOUNTER24H = 0xb98,
    BM_CSR_MHPMCOUNTER25H = 0xb99,
    BM_CSR_MHPMCOUNTER26H = 0xb9a,
    BM_CSR_MHPMCOUNTER27H = 0xb9b,
    BM_CSR_MHPMCOUNTER28H = 0xb9c,
    BM_CSR_MHPMCOUNTER29H = 0xb9d,
    BM_CSR_MHPMCOUNTER30H = 0xb9e,
    BM_CSR_MHPMCOUNTER31H = 0xb9f,
    BM_CSR_CYCLE          = 0xc00,
    BM_CSR_TIME           = 0xc01,
    BM_CSR_INSTRET        = 0xc02,
    BM_CSR_HPMCOUNTER3    = 0xc03,
    BM_CSR_HPMCOUNTER4    = 0xc04,
    BM_CSR_HPMCOUNTER5    = 0xc05,
    BM_CSR_HPMCOUNTER6    = 0xc06,
    BM_CSR_HPMCOUNTER7    = 0xc07,
    BM_CSR_HPMCOUNTER8    = 0xc08,
    BM_CSR_HPMCOUNTER9    = 0xc09,
    BM_CSR_HPMCOUNTER10   = 0xc0a,
    BM_CSR_HPMCOUNTER11   = 0xc0b,
    BM_CSR_HPMCOUNTER12   = 0xc0c,
    BM_CSR_HPMCOUNTER13   = 0xc0d,
    BM_CSR_HPMCOUNTER14   = 0xc0e,
    BM_CSR_HPMCOUNTER15   = 0xc0f,
    BM_CSR_HPMCOUNTER16   = 0xc10,
    BM_CSR_HPMCOUNTER17   = 0xc11,
    BM_CSR_HPMCOUNTER18   = 0xc12,
    BM_CSR_HPMCOUNTER19   = 0xc13,
    BM_CSR_HPMCOUNTER20   = 0xc14,
    BM_CSR_HPMCOUNTER21   = 0xc15,
    BM_CSR_HPMCOUNTER22   = 0xc16,
    BM_CSR_HPMCOUNTER23   = 0xc17,
    BM_CSR_HPMCOUNTER24   = 0xc18,
    BM_CSR_HPMCOUNTER25   = 0xc19,
    BM_CSR_HPMCOUNTER26   = 0xc1a,
    BM_CSR_HPMCOUNTER27   = 0xc1b,
    BM_CSR_HPMCOUNTER28   = 0xc1c,
    BM_CSR_HPMCOUNTER29   = 0xc1d,
    BM_CSR_HPMCOUNTER30   = 0xc1e,
    BM_CSR_HPMCOUNTER31   = 0xc1f,
    BM_CSR_CYCLEH         = 0xc80,
    BM_CSR_TIMEH          = 0xc81,
    BM_CSR_INSTRETH       = 0xc82,
    BM_CSR_HPMCOUNTER3H   = 0xc83,
    BM_CSR_HPMCOUNTER4H   = 0xc84,
    BM_CSR_HPMCOUNTER5H   = 0xc85,
    BM_CSR_HPMCOUNTER6H   = 0xc86,
    BM_CSR_HPMCOUNTER7H   = 0xc87,
    BM_CSR_HPMCOUNTER8H   = 0xc88,
    BM_CSR_HPMCOUNTER9H   = 0xc89,
    BM_CSR_HPMCOUNTER10H  = 0xc8a,
    BM_CSR_HPMCOUNTER11H  = 0xc8b,
    BM_CSR_HPMCOUNTER12H  = 0xc8c,
    BM_CSR_HPMCOUNTER13H  = 0xc8d,
    BM_CSR_HPMCOUNTER14H  = 0xc8e,
    BM_CSR_HPMCOUNTER15H  = 0xc8f,
    BM_CSR_HPMCOUNTER16H  = 0xc90,
    BM_CSR_HPMCOUNTER17H  = 0xc91,
    BM_CSR_HPMCOUNTER18H  = 0xc92,
    BM_CSR_HPMCOUNTER19H  = 0xc93,
    BM_CSR_HPMCOUNTER20H  = 0xc94,
    BM_CSR_HPMCOUNTER21H  = 0xc95,
    BM_CSR_HPMCOUNTER22H  = 0xc96,
    BM_CSR_HPMCOUNTER23H  = 0xc97,
    BM_CSR_HPMCOUNTER24H  = 0xc98,
    BM_CSR_HPMCOUNTER25H  = 0xc99,
    BM_CSR_HPMCOUNTER26H  = 0xc9a,
    BM_CSR_HPMCOUNTER27H  = 0xc9b,
    BM_CSR_HPMCOUNTER28H  = 0xc9c,
    BM_CSR_HPMCOUNTER29H  = 0xc9d,
    BM_CSR_HPMCOUNTER30H  = 0xc9e,
    BM_CSR_HPMCOUNTER31H  = 0xc9f,
    BM_CSR_SINTSTATUS     = 0xdb1,
    BM_CSR_MVENDORID      = 0xf11,
    BM_CSR_MARCHID        = 0xf12,
    BM_CSR_MIMPID         = 0xf13,
    BM_CSR_MHARTID        = 0xf14,
    BM_CSR_MINTSTATUS     = 0xfb1,
} bm_csr_id;

/**
 * \brief Obtain the value of a CSR
 *
 * \param csr CSR to read, bm_csr_id or a custom value defined by the target
 *
 * \return Value of the CSR
 */
xlen_t bm_csr_read(const int csr);

/**
 * \brief Set the value of a CSR
 *
 * \param csr CSR to write, bm_csr_id or a custom value defined by the target
 * \param value Value to write
 */
void bm_csr_write(const int csr, const xlen_t value);

/**
 * \brief Set bits of a CSR
 *
 * \param csr CSR to access, bm_csr_id or a custom value defined by the target
 * \param mask Bit mask to set
 */
void bm_csr_set_mask(const int csr, const xlen_t mask);

/**
 * \brief Clear bits of a CSR
 *
 * \param csr CSR to access, bm_csr_id or a custom value defined by the target
 * \param mask Bit mask to clear
 */
void bm_csr_clear_mask(const int csr, const xlen_t mask);

/**
 * \brief Functions for handling of custom, target-specific, CSRs
 */
#ifdef TARGET_HAS_CUSTOM_CSR
    #include <target_csr.h>
xlen_t target_csr_read(const int csr);
void   target_csr_write(const int csr, const xlen_t value);
void   target_csr_set_mask(const int csr, const xlen_t mask);
void   target_csr_clear_mask(const int csr, const xlen_t mask);
#endif

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_CSR_H */

/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TARGET_CSR_H
#define TARGET_CSR_H

/** \brief Enumeration of targets custom CSR IDs */
typedef enum {
    BM_CSR_SBPREDCTRL    = 0x5e8, // Custom branch prediction control register
    BM_CSR_MCFGCTRL      = 0x7e0, // Custom configuration control
    BM_CSR_MDCACHECTRL   = 0x7e5, // Custom data cache control register
    BM_CSR_MICACHECTRL   = 0x7e6, // Custom instruction cache control register
    BM_CSR_ML2CACHECTRL  = 0x7e7, // Custom L2 cache control register
    BM_CSR_MTLBCTRL      = 0x7e8, // Custom TLB control register
    BM_CSR_MTCMCFG       = 0x7e9, // TCM configuration register
    BM_CSR_MITCMBASEADDR = 0x7ea, // ITCM base address register
    BM_CSR_MDTCMBASEADDR = 0x7eb, // DTCM base address register
    BM_CSR_MCACHESTATUS  = 0xfe4, // Custom cache status register
} target_csr_id;

#endif /* TARGET_CSR_H */

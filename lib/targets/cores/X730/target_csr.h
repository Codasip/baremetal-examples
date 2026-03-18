/* Copyright 2024-2025 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TARGET_CSR_H
#define TARGET_CSR_H

/** \brief List of targets custom CSR IDs */
#define BM_CSR_SBPREDCTRL   0x5e8 // Custom branch prediction control register
#define BM_CSR_MCFGCTRL     0x7e0 // Custom configuration control
#define BM_CSR_MDCACHECTRL  0x7e5 // Custom data cache control register
#define BM_CSR_MICACHECTRL  0x7e6 // Custom instruction cache control register
#define BM_CSR_ML2CACHECTRL 0x7e7 // Custom L2 cache control register
#define BM_CSR_MTLBCTRL     0x7e8 // Custom TLB control register
#define BM_CSR_MCACHESTATUS 0xfe4 // Custom cache status register

#ifdef CONFIG_HAS_TCMS
    #define BM_CSR_MTCMCFG       0x7e9 // TCM configuration register
    #define BM_CSR_MITCMBASEADDR 0x7ea // ITCM base address register
    #define BM_CSR_MDTCMBASEADDR 0x7eb // DTCM base address register
#endif                                 // CONFIG_HAS_TCMS

#endif /* TARGET_CSR_H */

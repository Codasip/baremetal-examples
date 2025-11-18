/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TARGET_CSR_H
#define TARGET_CSR_H

/** \brief List of targets custom CSR IDs */
#define BM_CSR_MEXCAUSE 0x7D4 // Machine trap extra cause information

#ifdef TARGET_HAS_CACHE
    #define BM_CSR_ML1CACHE_START 0xfc0
#endif // TARGET_HAS_CACHE

#ifdef TARGET_HAS_TCM
    #define BM_CSR_MITCMEN   0x7cd
    #define BM_CSR_MDTCMEN   0x7d0
    #define BM_CSR_MITCMSIZE 0xfc2
    #define BM_CSR_MITCMADDR 0xfc3
    #define BM_CSR_MDTCMSIZE 0xfc4
    #define BM_CSR_MDTCMADDR 0xfc5
#endif // TARGET_HAS_TCM

#endif /* TARGET_CSR_H */

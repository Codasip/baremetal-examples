/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TARGET_CSR_H
#define TARGET_CSR_H

/** \brief Enumeration of targets custom CSR IDs */
typedef enum {
    BM_CSR_MITCMEN        = 0x7cd,
    BM_CSR_MDTCMEN        = 0x7d0,
    BM_CSR_MEXCAUSE       = 0x7D4, // Machine trap extra cause information
    BM_CSR_ML1CACHE_START = 0xfc0,
    BM_CSR_MITCMSIZE      = 0xfc2,
    BM_CSR_MITCMADDR      = 0xfc3,
    BM_CSR_MDTCMSIZE      = 0xfc4,
    BM_CSR_MDTCMADDR      = 0xfc5,
} target_csr_id;

#endif /* TARGET_CSR_H */

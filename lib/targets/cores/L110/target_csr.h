/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TARGET_CSR_H
#define TARGET_CSR_H

/** \brief Enumeration of targets custom CSR IDs */
typedef enum {
    BM_CSR_MEXCAUSE = 0x7D4, // Machine trap extra cause information
} target_csr_id;

#endif /* TARGET_CSR_H */

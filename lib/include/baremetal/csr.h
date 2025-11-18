/* Copyright 2023-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_CSR_H
#define BAREMETAL_CSR_H

#include "baremetal/common.h"

#include <baremetal/csr_register_map.h>
#include <target_csr.h>

#define BM_CSR_READ(csr, val)                                                \
    do                                                                       \
    {                                                                        \
        xlen_t __val;                                                        \
        __asm__ volatile("csrr %0, %1" : "=r"(__val) : "i"(csr) : "memory"); \
        val = __val;                                                         \
    } while (0)

#define BM_CSR_WRITE(csr, val)                                              \
    do                                                                      \
    {                                                                       \
        xlen_t __wv = (xlen_t)(val);                                        \
        __asm__ volatile("csrw %0, %1" : : "i"(csr), "r"(__wv) : "memory"); \
    } while (0)

#define BM_CSR_SET(csr, val)                                                \
    do                                                                      \
    {                                                                       \
        xlen_t __sv = (xlen_t)(val);                                        \
        __asm__ volatile("csrs %0, %1" : : "i"(csr), "r"(__sv) : "memory"); \
    } while (0)

#define BM_CSR_CLEAR(csr, val)                                              \
    do                                                                      \
    {                                                                       \
        xlen_t __cv = (xlen_t)(val);                                        \
        __asm__ volatile("csrc %0, %1" : : "i"(csr), "r"(__cv) : "memory"); \
    } while (0)

#define BM_CSR_UNDEFINED -1

// Typedef for BM CSR type
typedef xlen_t bm_csr_id_t;

#endif /* BAREMETAL_CSR_H */

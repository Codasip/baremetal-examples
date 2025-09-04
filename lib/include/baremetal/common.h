/* Copyright 2023-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_COMMON_H
#define BAREMETAL_COMMON_H

#include "core_info.h"
#include "platform_info.h"

#include <inttypes.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UNUSED __attribute__((unused))
#define USED   __attribute__((used))
#define WEAK   __attribute__((weak))

#if RISCV_XLEN == 32
typedef uint32_t xlen_t;
    #define BM_FMT_XLEN "0x%08" PRIx32
#elif RISCV_XLEN == 64
typedef uint64_t xlen_t;
    #define BM_FMT_XLEN "0x%016" PRIx64
#else
    #error "unsupported RISCV_XLEN"
#endif /* RISCV_XLEN == */

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_COMMON_H */

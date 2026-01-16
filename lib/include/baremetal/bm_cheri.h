/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BM_CHERI_H
#define BM_CHERI_H

#include <inttypes.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __CHERI_PURE_CAPABILITY__
    #include <cheriintrin.h>

    #define addr_to_bounded_data_ptr(base_addr, boundary_size, perms)                       \
        cheri_perms_and(                                                                    \
            cheri_bounds_set(cheri_address_set(cheri_ddc_get(), base_addr), boundary_size), \
            perms)

    #define addr_to_io_ptr(base_addr, boundary_size) \
        addr_to_bounded_data_ptr(base_addr, boundary_size, CHERI_PERM_READ | CHERI_PERM_WRITE)

    #if __riscv_clen == 64
        #define addr_to_code_ptr(base_addr) cheri_address_set(cheri_pcc_get(), base_addr)

    #else
        #define addr_to_code_ptr(base_addr) \
            cheri_perms_clear(cheri_address_set(cheri_pcc_get(), base_addr), CHERI_PERM_WRITE)
    #endif

    #define addr_to_data_ptr(base_addr, boundary_size) \
        addr_to_bounded_data_ptr(base_addr, boundary_size, CHERI_PERM_READ | CHERI_PERM_WRITE)

#else /* !__CHERI_PURE_CAPABILITY__ */
    #define addr_to_ptr(base_addr) \
        (void *)(uintptr_t)(base_addr) /* NOLINT(performance-no-int-to-ptr) */

    #define addr_to_io_ptr(base_addr, boundary_size)   addr_to_ptr(base_addr)

    #define addr_to_code_ptr(base_addr)                addr_to_ptr(base_addr)

    #define addr_to_data_ptr(base_addr, boundary_size) addr_to_ptr(base_addr)

#endif /* !__CHERI_PURE_CAPABILITY__ */

#ifdef __cplusplus
}
#endif

#endif /* BM_CHERI_H */

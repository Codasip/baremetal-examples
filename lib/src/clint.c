/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/clint.h"

#include "baremetal/common.h"
#include "baremetal/platform.h"

#include <stdint.h>

// Maximum numbers of MSIP and MTIMECMP registers (defined by specification)
#define CLINT_MAX_MSIPS     4095
#define CLINT_MAX_MTIMECMPS 4095

struct bm_clint_regs {
    volatile uint32_t MSIP[CLINT_MAX_MSIPS]; /**< (@ 0x0000) MSIP registers */
    uint8_t           _reserved1[0x4];
    volatile uint64_t MTIMECMP[CLINT_MAX_MTIMECMPS]; /**< (@ 0x4000) MTIMECMP registers */
    volatile uint64_t MTIME;                         /**< (@ 0xBFF8) MTIME register */
};

unsigned bm_clint_ticks_to_ms(const bm_clint_t *clint, uint64_t ticks)
{
    return ticks / (clint->freq / 1000);
}

uint64_t bm_clint_ms_to_ticks(const bm_clint_t *clint, unsigned milliseconds)
{
    return (uint64_t)milliseconds * (clint->freq / 1000);
}

uint64_t bm_clint_get_mtime(bm_clint_t *clint)
{
    return clint->regs->MTIME;
}

void bm_clint_set_mtimecmp(bm_clint_t *clint, unsigned hart_id, uint64_t time_value)
{
#if __riscv_xlen == 32
    // On 32-bit platforms, 64-bit writes are not necessarily atomic and ordering of accesses is not guaranteed.
    // Write a safe value (0xFFFFFFFF) to high word first to eliminate possible interrupt occurring in the middle.
    volatile uint32_t *reg_ptr = (uint32_t *)&clint->regs->MTIMECMP[hart_id];
    reg_ptr[1]                 = 0xFFFFFFFF;
    reg_ptr[0]                 = time_value & 0xFFFFFFFF;
    reg_ptr[1]                 = time_value >> 32;
#else
    clint->regs->MTIMECMP[hart_id] = time_value;
#endif
}

void bm_clint_arm_timer(bm_clint_t *clint, unsigned hart_id, uint64_t ticks_from_now)
{
    bm_clint_set_mtimecmp(clint, hart_id, bm_clint_get_mtime(clint) + ticks_from_now);
}

void bm_clint_rearm_timer(bm_clint_t *clint, unsigned hart_id, uint64_t delay_ticks)
{
    uint64_t current_timer = clint->regs->MTIMECMP[hart_id];

    bm_clint_set_mtimecmp(clint, hart_id, current_timer + delay_ticks);
}

void bm_clint_send_ipi(bm_clint_t *clint, unsigned hart_id)
{
    clint->regs->MSIP[hart_id] = 1;
}

void bm_clint_clear_ipi(bm_clint_t *clint, unsigned hart_id)
{
    clint->regs->MSIP[hart_id] = 0;
}

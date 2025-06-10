/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

/* Memories available for Hobgoblin v2 platform */

MEMORY
{
    boot_rom (!w) : org = 0x28000000, len = 0x00010000
    boot_ram (wxa) : org = 0x20030000, len = 0x0010000
    ram (wxa) : org = 0x20000000, len = 0x00080000
    ddr (wxa) : org = 0x80000000, len = 0x40000000
}

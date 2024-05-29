/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

/* Memories available for Hobgoblin (A730) platform */

MEMORY
{
    boot_rom (!w) : org = 0x10000000, len = 0x00010000
    boot_ram (wxa) : org = 0x20070000, len = 0x0010000
    ram (wxa) : org = 0x20000000, len = 0x00080000
    ddr (wxa) : org = 0x80000000, len = 0x40000000

    /* No TCMs */
    itcm (!w) : org = 0x0, len = 0x0
    dtcm (!x) : org = 0x0, len = 0x0
}

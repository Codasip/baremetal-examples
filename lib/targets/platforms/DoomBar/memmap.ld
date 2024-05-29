/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

/* Memories available for L31 platform */

MEMORY
{
    boot_rom (!w) : org = 0x10000000, len = 0x00010000
    boot_ram (wxa) : org = 0x10400000, len = 0x00010000
    ram (wxa) : org = 0x20000000, len = 0x00080000
    ddr (wxa) : org = 0x80000000, len = 0x40000000
    itcm (!w) : org = 0x02000000, len = 0x00004000
    dtcm (!x) : org = 0x03000000, len = 0x00004000
}

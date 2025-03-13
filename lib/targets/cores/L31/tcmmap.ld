/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

/* Memories available for L31 core */

MEMORY
{
    /* TCMs */
    itcm (!w) : org = 0x02000000, len = 0x4000
    dtcm (!x) : org = 0x03000000, len = 0x4000
}

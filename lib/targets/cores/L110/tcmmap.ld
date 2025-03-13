/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

/* Memories available for L110 core */

MEMORY
{
    /* No TCMs */
    itcm (!w) : org = 0x02000000, len = 0x0
    dtcm (!x) : org = 0x03000000, len = 0x0
}

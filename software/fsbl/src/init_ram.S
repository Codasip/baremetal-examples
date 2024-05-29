/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * This file provides functionality for initializing data sections in RAM memory, when their contents
 * are stored in a ROM memory.
 *
 * This is only needed if the executable is to be stored in the ROM memory, include this file to override
 * the default, blank, function definition in crt0.S file.
 */


/*
 * @brief Copy contents of the data sections from ROM to the right location in RAM
 *
 * Requires no parameters, just symbols defined in linker script:
 * - __data_begin      - start of data section in RAM
 * - __data_rom_start  - start of data section in ROM
 * - __data_rom_end    - end of data section in ROM
 */
init_ram:
    .global init_ram
    la a0, __data_begin
    la a1, __data_rom_start
    la a2, __data_rom_end
1:
    lw t1, 0(a1)
    sw t1, 0(a0)
    addi a0, a0, 4
    addi a1, a1, 4
    blt a1, a2, 1b
    ret


/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

/* Linker script for executables stored in ROM */
ENTRY (_start)

__TEXT_START_ADDR = DEFINED(_TEXT_START_ADDR) ? _TEXT_START_ADDR : ORIGIN(boot_rom);
__HEAP_SIZE = DEFINED(_HEAP_SIZE) ? _HEAP_SIZE : 0x0;
__STACK_SIZE = DEFINED(_STACK_SIZE) ? _STACK_SIZE : 0x4000;
__MP_EXTRA_STACK_SIZE = __STACK_SIZE * (DEFINED(_NUM_HARTS) ? _NUM_HARTS - 1 : 0);
__SCS_SIZE = DEFINED(_SCS_SIZE) ? _SCS_SIZE : 0x00000000;

SECTIONS
{
  /* CODE */
  .text __TEXT_START_ADDR : {
    KEEP (*(SORT(.crt*)))
    *(.text .text.*)
    *(.gnu.linkonce.t.*)
  } >boot_rom

  .init : ALIGN(8) {
    KEEP (*(.init))
  } >boot_rom =0

  .fini : ALIGN(8) {
    KEEP (*(.fini))
  } >boot_rom =0

  /* DATA */
  .ctors : ALIGN(8) {
    PROVIDE (__CTOR_LIST__ = .);
    PROVIDE (___CTOR_LIST__ = .);
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    PROVIDE (__CTOR_END__ = .);
    PROVIDE (___CTOR_END__ = .);
  } >boot_rom

  .dtors : ALIGN(8) {
    PROVIDE (__DTOR_LIST__ = .);
    PROVIDE (___DTOR_LIST__ = .);
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    PROVIDE (__DTOR_END__ = .);
    PROVIDE (___DTOR_END__ = .);
  } >boot_rom

  .rodata : ALIGN(8) {
    *(.srodata .srodata.*)
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
  } >boot_rom

  .eh_frame : ALIGN(8) {
    KEEP(*(.eh_frame))
  } >boot_rom

  .eh_frame_hdr : ALIGN(8) {
    KEEP(*(.eh_frame_hdr))
  } >boot_rom

  .gcc_except_table : ALIGN(8) {
    KEEP(*(.gcc_except_table .gcc_except_table.*))
  } >boot_rom

  .data : ALIGN(8) {
    __data_begin = .;
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(8);
    __sdata_begin = .;
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
  } >boot_ram AT>boot_rom

  .bss (NOLOAD) : ALIGN(8) {
    __bss_start = .;
    *(.sbss .sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(8);
    __bss_end = .;
  } >boot_ram

  .scs (NOLOAD) : ALIGN(16) {
    . += __SCS_SIZE;
  } >boot_ram

  .heap (NOLOAD) : ALIGN(8) {
    _heap = .;
    . += __HEAP_SIZE;
    _heap_end = .;
  } >boot_ram

  .stack (NOLOAD) : ALIGN(16) {
    . += __MP_EXTRA_STACK_SIZE;
    _stack_end = .;
    . += __STACK_SIZE;
    . = ALIGN(16);
    _stack = .;
  } >boot_ram

  __global_pointer$ = MIN(__sdata_begin + 0x800,
                      MAX(__data_begin + 0x800, __bss_end - 0x800));
  _scs = SIZEOF(.scs) > 0 ? ADDR(.scs) + SIZEOF(.scs) : 0;
  __data_rom_start = LOADADDR(.data);
  __data_rom_end = LOADADDR(.data) + SIZEOF(.data);
  _end = .;
}

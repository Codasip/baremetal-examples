/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

/* Default linker script, for normal executables */

ENTRY (_start)

__TEXT_START_ADDR = DEFINED(_TEXT_START_ADDR) ? _TEXT_START_ADDR : ORIGIN(ram);
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
  } >ram

  .init : ALIGN(8) {
    KEEP (*(.init))
  } >ram =0

  .fini : ALIGN(8) {
    KEEP (*(.fini))
  } >ram =0

  /* DATA */
  .ctors : ALIGN(8) {
    PROVIDE (__CTOR_LIST__ = .);
    PROVIDE (___CTOR_LIST__ = .);
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    PROVIDE (__CTOR_END__ = .);
    PROVIDE (___CTOR_END__ = .);
  } >ram

  .dtors : ALIGN(8) {
    PROVIDE (__DTOR_LIST__ = .);
    PROVIDE (___DTOR_LIST__ = .);
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    PROVIDE (__DTOR_END__ = .);
    PROVIDE (___DTOR_END__ = .);
  } >ram

  .data : ALIGN(8) {
    __data_begin = .;
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(8);
    __sdata_begin = .;
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
  } >ram

  .bss (NOLOAD) : ALIGN(8) {
    __bss_start = .;
    *(.sbss .sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(8);
    __bss_end = .;
  } >ram

  .rodata : ALIGN(8) {
    *(.srodata .srodata.*)
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
  } >ram

  .eh_frame : ALIGN(8) {
    __eh_frame_start = .;
    KEEP(*(.eh_frame))
    __eh_frame_end = .;

  } >ram

  .eh_frame_hdr : ALIGN(8) {
    KEEP(*(.eh_frame_hdr))
  } >ram

  .gcc_except_table : ALIGN(8) {
    KEEP(*(.gcc_except_table .gcc_except_table.*))
  } >ram

  .scs (NOLOAD) : ALIGN(16) {
    . += __SCS_SIZE;
  } >ram

  /* Code to be placed to ITCM */
  .itcm : ALIGN(8) {
    *(.itcm .itcm.*)
  } >itcm AT>ram

  /* Data to be placed to DTCM */
  .dtcm : ALIGN(8) {
    *(.dtcm .dtcm.*)
  } >dtcm AT>ram

  .heap (NOLOAD) : ALIGN(8) {
    _heap = .;
    . += __HEAP_SIZE;
    _heap_end = .;
  } >ram

  .stack (NOLOAD) : ALIGN(16) {
    . += __MP_EXTRA_STACK_SIZE;
    _stack_end = .;
    . += __STACK_SIZE;
    . = ALIGN(16);
    _stack = .;
  } >ram

  __global_pointer$ = MIN(__sdata_begin + 0x800,
                      MAX(__data_begin + 0x800, __bss_end - 0x800));
  _scs = SIZEOF(.scs) > 0 ? ADDR(.scs) + SIZEOF(.scs) : 0;
  __eh_frame_hdr_start = SIZEOF(.eh_frame_hdr) > 0 ? ADDR(.eh_frame_hdr) : 0;
  __eh_frame_hdr_end = SIZEOF(.eh_frame_hdr) > 0 ? . : 0;
  __itcm_data_start = LOADADDR(.itcm);
  __itcm_data_end = LOADADDR(.itcm) + SIZEOF(.itcm);
  __dtcm_data_start = LOADADDR(.dtcm);
  __dtcm_data_end = LOADADDR(.dtcm) + SIZEOF(.dtcm);
  _end = .;
}

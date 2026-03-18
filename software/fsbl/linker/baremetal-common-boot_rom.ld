/* Expose MEMORY region info to C */
__boot_rom_start  = ORIGIN(boot_rom);
__boot_rom_size   = LENGTH(boot_rom);

__boot_ram_start  = ORIGIN(boot_ram);
__boot_ram_size   = LENGTH(boot_ram);

__ram_start       = ORIGIN(ram);
__ram_size        = LENGTH(ram);

__ddr_start       = ORIGIN(ddr);
__ddr_size        = LENGTH(ddr);

SECTIONS
{
    .text : {
        *(.text.init)
        *(.text.unlikely .text.unlikely.*)
        *(.text.startup .text.startup.*)
        *(.text .text.*)
        *(.gnu.linkonce.t.*)
    } >boot_rom
    PROVIDE (_etext = .);

    /* Static Thread Local Storage template */
    .tdata : {
        PROVIDE_HIDDEN (__tdata_start = .);
        *(.tdata .tdata.*)
        *(.gnu.linkonce.td.*)
        PROVIDE_HIDDEN (__tdata_end = .);
    } >boot_ram

    .tbss (NOLOAD) : {
        PROVIDE_HIDDEN (__tbss_start = .);
	*(.tbss .tbss.*)
        *(.gnu.linkonce.tb.*)
        *(.tcommon)
        PROVIDE_HIDDEN (__tbss_end = .);
    } >boot_ram

    .rodata : {
        *(.rodata .rodata.*)
        *(.gnu.linkonce.r.*)
    } >boot_rom

    .preinit_array : {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } >boot_rom

    .init_array : {
	/* Linker generates an implicit:
	 * PROVIDE_HIDDEN (__init_array_start = .);
	 * here, but gets the symbol size right. Making it explicit would be size 0. */
	KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
	/* PROVIDE_HIDDEN (__init_array_end = .); */
    } >boot_rom

    .fini_array : {
	/* Linker generates an implicit:
	 * PROVIDE_HIDDEN (__fini_array_start = .);
	 * here, but gets the symbol size right. Making it explicit would be size 0. */
	KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
	/* PROVIDE_HIDDEN (__fini_array_end = .); */
    } >boot_rom

    .ctors : {
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    } >boot_rom

    .dtors : {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    } >boot_rom

    eh_frame :
    {
	__eh_frame_start = .;
	KEEP(*(.eh_frame))
	__eh_frame_end = .;
    } >boot_rom

    .eh_frame_hdr :
    {
	KEEP(*(.eh_frame_hdr))
    } >boot_rom

    __eh_frame_hdr_start = SIZEOF(.eh_frame_hdr) > 0 ? ADDR(.eh_frame_hdr) : 0;
    __eh_frame_hdr_end = SIZEOF(.eh_frame_hdr) > 0 ? . : 0;

    /* Host-Target Interface */
    .htif ALIGN(0x40) : {
        *(.htif)
    } >boot_ram

    .data : ALIGN (8) {
        __data_begin = .;
        *(.data)
        *(.gnu.linkonce.d.*)
    } >boot_ram AT>boot_rom

    .sdata : {
        PROVIDE (__global_pointer$ = . + 0x800);
        *(.sdata .sdata.*)
        *(.gnu.linkonce.s.*)
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)
    } >boot_ram

    PROVIDE (_edata = .);

    .bss : ALIGN (8) {
        PROVIDE_HIDDEN (__bss_start = .);
        *(.sbss .sbss.*)
        *(.gnu.linkonce.sb.*)
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        PROVIDE_HIDDEN (__bss_end = .);
    } >boot_ram

    . = ALIGN (8);
    PROVIDE (_end = .);

    /* Reserve heap space */
    PROVIDE (__heap_start = .);
    . += __heap_size;
    . = ALIGN (4K);
    PROVIDE (__heap_end = .);

    /* Place Thread Control Block (TCB) at bottom of hart stack */
    PROVIDE (__tcb_size = __tbss_end - __tdata_start);
    PROVIDE (__stack_align = MAX (ALIGNOF (.tdata), 0x10));
    PROVIDE (__stack_start = ALIGN (__stack_align));
    PROVIDE (__stack_shift = LOG2CEIL( ALIGN (__stack_size_min + __tcb_size, __stack_align)));
    PROVIDE (__stack_size = 1 << __stack_shift);
    __data_rom_start = LOADADDR(.data);
    __data_rom_end = LOADADDR(.data) + SIZEOF(.data);
}

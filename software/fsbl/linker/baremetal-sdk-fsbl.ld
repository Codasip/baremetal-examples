OUTPUT_ARCH( "riscv" )
ENTRY(_start)

/*
 * Memory layout
 */
INCLUDE memmap.ld

/* Address of peripherials */
PROVIDE( __uart16550_base = 0x10101000 );
PROVIDE( __gpio_sd_base = 0x10310000 );

/* Default boot hart */
PROVIDE (__boot_hart = 0);

/* Keep under a power of 2 to reserve space for TCB */
PROVIDE (__stack_size_min = 24K);

PROVIDE (__heap_size = 64K);

INCLUDE baremetal-common-boot_rom.ld

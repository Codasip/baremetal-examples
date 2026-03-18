# Changelog

This file contains all the major changes in different releases.

## v1.1.9

### Features
- add RISC-V CHERI architecture (Y-Extension) support for FSBL and TSBL
- add support for V739-Sphene
- add support for V735-Amber

### Miscellaneous
- minor typo and align cleanups

### Fix
- don't reset CLIC config when enabling or disabling interrupts
- fix and clarify CLIC interrupt API behavior
- fix interrupt initialization in examples

## v1.1.8

### Features
- add RISC-V CHERI architecture (Y-Extension) support
- add support for KR260 FPGA board
- add initialization of core registers for L110 and L150 cores
- add explicitly set section for Zcmt extension in FSBL
- add support for A730-Flint

### Miscellaneous
- use register context parameter for interrupt handler
- drop obsolete TARGET_LINUX_SUPPORT for V730
- minor configuration and build system cleanups
- set `CONFIG_PLATFORM` to Version 2 by default. (Hobgoblin/Hoobs v2 platform is now used by default; HOB_PLATFORM_VERSION configuration variable has been removed).
- add PLIC to `REQUIRES` list for `privilege-interrupts` and `privilege-interrupts-delegated` demos

### Fix
- fix typo in variable name used for ABI selection (`CONFIG_CC_USE_DEFAULT_MABI` -> `CONFIG_CC_USE_DEFAULT_ABI`)

## v1.1.7

### Feature
- add support for V730 core family
- add support for V739-Spinel
- clean up CSR read/write mechanisms:
    - improved CSR access wrapper functions (`CSR_READ()` renamed to `BM_CSR_READ()`)
    - updated API for accessing CSRs at different privilege levels: `bm_priv_csr_read()`, `bm_priv_csr_write()`, `bm_priv_csr_set()`, `bm_priv_csr_clear()`

### Miscellaneous
- minor code cleanup and adjustments
- rename config-X730-mp4 to config-X730-lux
- ARCH and ABI are based on core config files
- add helper functions and macros: `BM_ARRAY_ELEMENTS()`, `bm_fatal_check_index()`
- remove config flag `TARGET_HAS_CUSTOM_CSR`

### Fix
- fix bm_interrupt_tvec_get_mode() function
- fix sloppy assembly usage in interrupt.c file

## v1.1.6

### Feature
- clean up disassembly file
- add support for building an app from outside the SDK folder

### Miscellaneous
- minor code cleanup and adjustments

### Fix
- fix build warnings
- fix issues with Hobgoblin v2 version setting
- fix interrupt handling for multi-core systems

## v1.1.5

### Feature
- add demos: sd-card-demo, id-register-demo, CMSIS-RTX
- add support for L110-Lyra
- add support for L110/L150 simulator exit syscall
- enable A-Extension for L730
- force external repos to be specified explicitly
- make memory and interrupt map more flexible
- add target to create disassembly
- avoid using __riscv_xlen directly

### Miscellaneous
- cleanup in FSBL
- update copyright
- avoid duplicating fatfs lib

## v1.1.4

### Feature
- add L150-Libra platform support
- add Hobgoblin 2.0 memory map support
- refactor memory map & interrupt map

### Fix
- fix warnings
- fix variable naming issues
- cleanup interrupts
- fsbl: trap early

### Miscellaneous
- FSBL: add Third Stage BootLoader (TSBL)
- update version
- bm_msg_args cleanup

## v1.1.3

### Feature
- add L730-Amethyst support
- add L730-Ruby support

### Fix
- fix SPI and FAT FS to correctly power cycle SD card

### Miscellaneous
- update x730 compiler march

## v1.1.2

### Feature
- add X730 CHERI platform support
- add L110 Codasip vs RISC-V target
- add L31 support
- add L730-Pearl support
- enable A730 PMP
- add CMSIS-FreeRTOS demo

### Fix
- fix no-TCM link error
- fix Makefile issues

### Miscellaneous
- cleanup in FSBL
- add verbose fatal messages
- update formatting rules
- whitespace cleanup

## v1.1.0

### Feature
- add L110 CLIC support
- add FreeRTOS

### Fix
- L110: minor fixes
- fix documentation for GitHub
- fix VCU118 SD-card issue


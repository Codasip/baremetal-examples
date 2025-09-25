# Changelog

This file contains all the major changes in different releases.

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


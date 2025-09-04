# CODASIP FIRST STAGE BOOTLOADER (FSBL)

Simple example bootloader, intended as the first piece of code executed
in Codasip platforms after reset. As such, it primarily meant to be located
in a ROM memory. Codasip platforms also contain a small region of RAM memory
for use by the bootloader. Alternatively, the demo also supports its execution
from the main RAM memory for testing purposes.

# CODASIP THIRD STAGE BOOTLOADER (TSBL)

There is an additional linker script that places all of the FSBL in RAM at a high address.
This is useful as a third stage bootloader if the first stage is, for instance, Codasip's Secure-Boot
(which has two stages). This TSBL can be encrypted and signed to be booted using Secure-Boot.
Then the TSBL allows the developer to test many unsigned and unencrypted applications quickly
without the aditional steps and tools required to sign and encrypt them for Secure-Boot.

## Functional Overview

The FSBL loads configuration and payloads from a FAT32 formatted microSD card:

1. Execution starts by relocating initialized data section from ROM to RAM,
   and clearing the BSS section.
2. Configuration file `config.txt` is located in the root folder of the microSD
   card filesystem.
   NOTE: If the `config.txt` file does bot exist, then the user is shown a list
   of `*.bin` files on the SD-Card and asked to select one them to load
   at the fixed address of 0x20000000.
3. Based on the configuration, payloads are loaded from the microSD card
   at specified addresses in the memory.
4. Execution continues at one of the payloads, selected by the configuration
   file.

## Configuration File

FSBL configuration file is a ASCII formated file, which contains a number
of entries in the following format:

`[<number>] <file> <address> ["BOOT" | "FDT" | "NXT"]`

Where:
- `number` is a decimal number from 0 to the number of available GPIO switches,
- `file` is a path to a file without spaces,
- `address` is a hexadecimal address in a writable memory.

All the other lines have to start with `#` and are treated as comments, white
spaces and blank lines are ignored.

FSBL uses these configuration entries in the following manner:
1. Select all entries without the `number` parameter present, and those with
   a `number`, for which the corresponding switch is flipped on.
2. Load specified files at the given addresses, in the order of entries.
3. Start execution on the address of first entry denoted `BOOT`.

Optionally, for targets supporting Linux, configuration entries can also be
denoted `FDT` or `NXT`. These payloads are then used during Linux boot:
- The `address` of first entry denoted `FDT` is loaded to `a1` register,
  to instruct OpenSBI to use this payload as a device tree,
- The `address` of first entry denoted `NXT` is loaded to `a2` register,
  to instruct OpenSBI to use this payload as a next stage bootloader,

## Example Configuration File

The following configuration file allows to load and execute one of the demos,
depending on the position of the GPIO switches.

```
0 /dhrystone-demo.bin 0x20000000 BOOT
1 /coremark-demo.bin  0x20000000 BOOT
```

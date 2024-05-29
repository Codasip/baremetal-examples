# Bare-metal Examples

This repository contains bare-metal software developed by Codasip to run on Codasip RISC-V processors in Codasip SDK IA/CA simulators or on development FPGA boards with Codasip FPGA platform.

This _README_ explains how to build individual demo applications.
For instructions how to run them in the simulator, please refer to SDK documentation for the particular Codasip Processor.
For instructions how to run them in an FPGA, please refer to the documentation for the particular Codasip FPGA Platform.

## Repository structure

```
baremetal-examples
  |- doc/doc.md      # Documentation
  |- lib/            # Common library (HAL) sources
    |- include/      # Header files
    |- linker/       # Linker scripts
    |- src/          # Generic source files
    |- startup/      # Startup assembly files
    |- syscalls/     # Syscalls sources for different environments
    |- targets
       |- configs/   # Prepared (example) target configuration files
       |- cores/     # Core specific files
       |- platforms/ # Platform specific files
  |- share/          # Support files and scripts (Makefiles)
  |- software/       # Software applications (demos)
  |- README.md
  |- config.mk       # Configuration file
  |- Makefile        # Top level makefile, also used from Codasip Studio (CodeSpace)
```

## Quick start

### Building with Codasip Studio (CodeSpace)

1. Import Codasip SDK for the processor you are working with
2. Import baremetal examples directory using _Existing Code as Makefile Project_ option
3. Assign the IA Codasip SDK to the project
4. Make sure that file _config.mk_ is configured correctly:
   1. Remove or comment out the first line with error message
   2. Uncomment appropriate environment setting, see Environments section
   3. Uncomment include of appropriate target config file
   4. Uncomment selected demo application setting
5. Build the project

### Building from command-line (terminal)

1. Install `make`
2. Install a RISC-V toolchain (assembler, compiler, linker)
3. Provide its prefix in the `$SDK_PREFIX` variable, and make sure the tools are available in `$PATH`
4. Make sure that file _config.mk_  is configured correctly:
   1. Remove or comment out the first line with error message
   2. Uncomment appropriate environment setting, see Environments section
   3. Uncomment include of appropriate config file
   4. Uncomment selected demo application setting
5. Build the application by running `make clean all <bin>`.

## Configuring toolchain

It must be possible to run one of the following (build system will try the other options unless `$CC_NAME` is defined, which should therefore be prefered to avoid warnings):

```sh
${SDK_PREFIX}-clang --version
${SDK_PREFIX}clang --version
${SDK_PREFIX}-gcc --version
${SDK_PREFIX}gcc --version
${SDK_PREFIX}${CC_NAME} --version
```

Additionally, for building the `bin` make target, it must be possible to run one of:

```sh
${SDK_PREFIX}-objcopy --version
${SDK_PREFIX}objcopy --version
```

For example, to setup a toolchain from command line:

```sh
PATH=$PATH:/path/to/toolchain/folder/bin
export SDK_PREFIX=riscv32-unknown-elf-
export CC_NAME=gcc
```

Alternatively, it is also possible to configure a toolchain in _config.mk_, for example by adding the following at the start of the file:

```
PATH := ${PATH}:/path/to/toolchain/folder/bin
SDK_PREFIX = riscv32-unknown-elf-
CC_NAME = gcc
```

## Environments

Variable `CONFIG_ENVIRONMENT` is used in order to build an application for different environments. The environments differ in the memory map, peripherals that they provide, and in the mechanism of providing system calls (syscalls), notably I/O (console). Supported values are:

*SIMULATOR*:
- Application targets Codasip IA/CA simulator.
- Only the processor and a single memory region is simulated, no peripherals.
- Syscalls are provided through (simulated) Nexus debugger.

*FPGA_SEMIHOSTING*:
- Application targets Codasip FPGA Platform.
- Syscalls are implemented over semihosting through JTAG.
  - Debugger with RISC-V semihosting support (e.g. OpenOCD) must be running on the host system connected to the JTAG, and configured with semihosting enabled (for OpenOCD, use command `arm semihosting enable`).

*FPGA_UART*:
- Application targets Codasip FPGA Platform.
- I/O (console) is implemented using UART peripheral.
  - Terminal emulator application (e.g. PuTTy or Picocom) must be running on the host system, connected to the serial device (COM / TTY) representing the UART on the FPGA development board.
  - Default UART configuration is `8N1` (same as deafult for Picocom), and the default baudrate is `115200`. These settings can be adjusted in _lib/syscalls/sys_uart.c_, and must match the terminal emulator settings.

It is possible to either set the value in config file, or override the variable from command line:

```sh
make CONFIG_ENVIRONMENT=FPGA_SEMIHOSTING
```
## Targets

A number of cores and platforms are supported. The folder _lib/targets/configs_ contains example target configuration files. These can be used by including in top-level _config.mk_, or instead of the top-level config, by specifying `CONFIG_FILE` variable (settings from _config.mk_ will not be applied in this case):

```sh
make CONFIG_FILE=lib/targets/configs/config-{target configuration}.mk
```

### Defining custom targets

If the target core and the target platform are both already supported, a new configuration file can be created setting appropriate options, similar to ones in the `lib/targets/configs` folder. Alternatively, an existing configuration file can be modified. The target configuration file needs to be included in the top-level `config.mk`, or set in the `CONFIG_FILE` variable.

If either the target core or the target platform is not fully supported, for example when it was customized, the core/platform specific files in _lib/targets/cores/\<core\>_ or _lib/targets/platforms/\<platform\>_ need to be modified. Alternatively, a new core or platform folder can be created, in which case the variable `CONFIG_PROCESSOR` or `CONFIG_PLATFORM` simply needs to be assigned the folder name.

## Software demos

Most of the provided demos contain a single source file and a _Makefile_ which includes shared makefiles configuring the build, so demos can utilize as much as possible from the shared library and build system. Demos therefore do not have to contain any linker or startup scripts, or syscalls.

A notable exception is the First Stage Bootloader (FSBL) demo. FSBL uses a custom syscalls to facilitate SD card filestem, and also requires a custom linker script, as it needs to be linked in ROM instead of RAM.

### Creating custom demos

Additional demos can be created by the user, and integrated with the build system:
- define `DEMO_APP` variable so that `software/$(DEMO_APP)/Makefile` resolves to the demo's _Makefile_ (the `DEMO_APP` variable can be set in the top-level _config.mk_ file or from command line),
- in the demo's _Makefile_, include the _share/app.mk_ for basic demos, or _share/common.mk_ if a more specialized build configuration is required (see FSBL demo).


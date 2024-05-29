# Bare-metal Examples

This document is intended for an in depth description of the build system, various APIs provided by the bare-metal library, and corresponding demos utilizing these APIs.

For quick-start instructions or basic configuration information, please see _README_ in the top-level of the repository.

## Build System

The Bare-metal Examples repository uses the _make_ build system which is configurable either using environment variables or from the configuration makefiles included during the build. A number of different makefiles contribute to the build process:

Target core and platform (SoC) makefiles _lib/targets/cores/\<core\>/core.mk_ and _lib/targets/platforms/\<platform\>/platform.mk_:
- Configures which library sources or extra target-specific sources are built for the given target by extending the `BM_SOURCES` variable.
- Sets variable `PROVIDES` with specific features supported on the target.
- Sets target specific preprocessor defines (`CPPFLAGS`), these configure both library and demo sources.

Target configuration files _lib/targets/configs/config-\<target\>.mk_:
- Selects the target core and platform by setting variables `CONFIG_PROCESSOR` and `CONFIG_PLATFORM`.
- If the corresponding _platform.mk_ or _core.mk_ allows for configuration options (e.g. presence of an FPU or number of PMP registers), these are selected as well.

Generic build files _share/common.mk_ and _share/app.mk_:
- Includes a configuration file set in `CONFIG_FILE` variable (if left unset, the default _config.mk_ from top-level is used)
- Includes _lib/targets/cores/${CONFIG_PROCESSOR}/core.mk_ and _lib/targets/platforms/${CONFIG_PLATFORM}/platform.mk_ files.
- Compares variables `PROVIDES` (set by targets) and `REQUIRES` (set by demos), and abort the build if a requirement is not satisfied.
- Depending on the `CONFIG_ENVIRONMENT` variable, syscalls source file from _lib/syscalls_ folder is added to the build.
- Defines make build targets `all` (demo in ELF format), `bin` (demo in binary format), `clean` and `run` (if target environment is Codasip IA/CA simulator, which must be set in `SIM` variable).

Every demo application contains its own _Makefile_, which:
- Includes generic build files.
- Sets variable `REQUIRES` in case a specific target feature is required for the demo to work.
- If needed, sets build variables, such as compiler flags (`CFLAGS`).

Top-level _Makefile_:
- Allows build from top-level directory, simplifying workflow with Codasip Codespace.
- Includes _Makefile_ for a demo set in the `DEMO_APP` variable.

Top-level _config.mk_:
- Sets other configuration options.

### Example build commands

As explained in the previous section, it is possible to build both from the top-level and from the demo directory, without need to actually modify the top-level _config.mk_ file:

```sh
make SDK_PREFIX=riscv32-unknown-elf- CC_NAME=gcc CONFIG_FILE=lib/targets/configs/config-a730-default.mk DEMO_APP=hello-world CONFIG_ENVIRONMENT=SIMULATOR clean all
```

```sh
cd software/hello-world
make SDK_PREFIX=riscv32-unknown-elf- CC_NAME=gcc CONFIG_FILE=../../lib/targets/configs/config-a730-default.mk CONFIG_ENVIRONMENT=SIMULATOR clean all
```

On the other hand, it is also possible to configure all options in the _config.mk_, e.g.:

```
SDK_PREFIX=/<path to riscv toolchain>/riscv32-unknown-elf-
CC_NAME=gcc
CONFIG_ENVIRONMENT=FPGA_UART
DEMO_APP=hello-world
include lib/targets/configs/config-a730-default.mk
```

Such configuration file allows building by simply running:

```sh
make clean all
```

### List of target-specific defines

The following preprocessor defines are currently passed during build:
- `TARGET_CLK_FREQ` - core clock frequency
- `TARGET_CORE_NAME` - string with core name
- `TARGET_EXT_N` - cores implementing N extension
- `TARGET_EXT_S` - cores implementing S extension
- `TARGET_EXT_U` - cores implementing U extension
- `TARGET_GPIO_PORT_<LED/SWITCH/BUTTON><N>` - port numbers for IO GPIO
- `TARGET_GPIO_SD_<CARD_DETECT/POWER_ENABLE/FAST_CLOCK>` - port number for SD GPIO
- `TARGET_HAS_CACHE` - cores with Codasip cache management
- `TARGET_HAS_CUSTOM_CSR` - cores implementing custom CSR registers
- `TARGET_HAS_HPM` - cores with HPM counters
- `TARGET_HAS_PIC` - cores with Codasip Programmable Interrupt Controller
  - `TARGET_PIC_NUM_INTERRUPTS` - number of PIC interrupts
- `TARGET_HAS_PLIC` - platforms with Codasip Platform-Level Interrupt Controller
- `TARGET_HAS_PMP` - cores implementing PMPs
  - `TARGET_PMP_NUM_REGIONS` - number of PMP regions
- `TARGET_HAS_TCM` - cores with Codasip Tightly Coupled Memories
- `TARGET_LINUX_SUPPORT` - Linux capable cores
- `TARGET_NUM_HARTS` - number of harts on the target core/cluster
- `TARGET_PLATFORM_FREQ` - platform (peripheral) clock frequency
- `TARGET_PLATFORM_NAME` - string with platform name
- `TARGET_SIMULATION` - target environment is a Codasip simulator

### List of target-specific features

The following are currently set by the "requires/provides" mechanism described previously:
- `aead` - platforms with AEAD peripheral
- `atomics` - cores implementing the RISC-V A extension
- `l31cache` - L31 cores with Codasip cache management
- `clint` - platforms with a Codasip Core Local Interrupter
- `flash` - platforms with a Xilinx SPI peripheral connected to the on-board FLASH memory
- `gpio_io` - platforms with a Xilinx GPIO peripheral connected to the on-board LEDs and switches
- `hpm` - cores with HPM counters
- `i2c_pwr` - platforms with Xilinx I2C peripheral connected to INA219 power monitoring sensors
- `supervisor_mode` - cores implementing the S extension
- `user_mode` - cores implementing the U extension
- `pic` - cores with Codasip Programmable Interrupt Controller
- `plic` - platforms with a Codasip Platform Level Interrupt Controller
- `pmp` - cores implementing Physical Memory Protection
- `sdcard` - platforms with Xilinx SPI and GPIO peripherals connected to the on-board microSD card slot
- `tcm` - cores with Codasip Tightly Coupled Memories
- `trng` - platforms with Codasip True Random Number Generator
- `uart` - platforms with Xilinx UART peripheral

## Supported targets

### Cores

Folders _lib/targets/cores/\<core\>_ hold core-specific sources. The build can be configured to include any sources in the _core.mk_ file. However, certain contents are required in the target core directories if specific features are provided:

- Cores requiring some extra startup instructions define this in the `_core_init` routine (for example see: _lib/targets/cores/A730/core_init.S_), called from _lib/startup/crt0.S_.
- Cores with additional custom CSR registers define functions `target_csr_read`, `target_csr_write`, `target_csr_set_mask` and `target_csr_clear_mask` (for example see: _lib/targets/cores/L31/target_csr.c_), following the declarations in _lib/include/baremetal/csr.h_. Header file _lib/targets/cores/\<core\>/target_csr.h_ provides the enumeration of the custom CSRs.
- Cores implementing Hardware Performance Monitor (HPM) counters define functions `bm_hpmcounter_start`, `bm_hpmcounter_stop`, `bm_hpmcounter_pause`, `bm_hpmcounter_resume` and `bm_hpmcounter_read` (for example see: _lib/targets/cores/A730/target_hpm.c_), following the declarations in _lib/include/baremetal/hpm.h_. Header _lib/targets/cores/\<core\>/target_hpm.h_ provides the enumeration of supported HPM events.

Please refer to the relevant _README_ files for supported core configuration options:

- [A730](lib/targets/cores/A730/README.md)
- [L31](lib/targets/cores/L31/README.md)

### Platforms

Folders _lib/targets/platforms/\<platform\>_ hold platform-specific sources. Required contents are:

- File _memmap.ld_ defines the valid memory regions for the platform using the linker script `MEMORY` command. Expected region names are `boot_rom` and `boot_ram` (used from FSBL), `ram` and `ddr` (for generic use) and `itcm` and `dtcm` (for Instruction and Data Tightly Coupled Memories). Note: using `ddr`, which is usually located in the upper address space, may require passing `-mcmodel=medany` option to compiler (in `CFLAGS` variable).
- Target platforms define function `target_peripheral_get` which provids information about available peripherals. The function is declared in _lib/include/baremetal/platform.h_, together with an enumeration of the supported peripherals.

The supported target platforms are:

- [DoomBar](lib/targets/platforms/DoomBar/README.md)
- [Hobgoblin](lib/targets/platforms/Hobgoblin/README.md)
- [Inferno](lib/targets/platforms/Inferno/README.md)

## APIs and software demos

Even the simplest demo is built together with the library, which allows the user to configure the I/O (console) by setting the `CONFIG_ENVIRONMENT` variable as described previously. Although, this is not in the demo code (as it is has a global default configuration in _config.mk_ which can be overriden in the local _Makefile_, if required):

- [Hello-World demo](software/hello-world/README.md)

More complicated demos include a range of library includes and calls to library functions, documented in the subsequent sections. For instance, the OOB demo prints out the Codasip logo on the console and blinks the LEDs, while the FSBL loads and executes payloads from the microSD card:

- [Out-Of-the-Box demo](software/oob-demo/README.md)
- [First Stage BootLoader](software/fsbl/README.md)


### Basic core functionality

The bare-metal library provides functions for manipulating the Control and Status Registers (CSRs), parametrized by register number, as well as an enumeration holding the register numbers (see _lib/include/baremetal/csr.h_). This, for example, allows ease of defining and using custom CSRs even without the support of the compiler toolchain.

Support for counters is similar, with the library providing an enumeration of counters and functions for counter manipulation (see _lib/include/baremetal/counter.h_). This allows a layer of abstraction hiding away the difference between 64-bit and 32-bit RISC-V cores. For instance, on the 32-bit core, the 64-bit counter register values are split into two 32-bit CSRs, but the abstraction layer provides a 64-bit interface for 32-bit as well as 64-bit cores.

Handling of HPM counters is target specified, however the underlying principle is the same. The user-facing API (see _lib/include/baremetal/hpm.h_) allows operating the HPM counters by specifying only the desired HPM event.

Lastly, the Physical Memory Protection (PMP) bare-metal API (see _lib/include/baremetal/pmp.h_) allows the configurion of PMP regions.

For example usage, please see the relevant demos:

- [Counter demo](software/counter-demo/README.md)
- [CSR demo](software/csr-demo/README.md)
- [HPMcounter demo](software/hpmcounter-demo/README.md)
- [PMP demo](software/pmp-demo/README.md)
- [Timing demo](software/timing-demo/README.md)

### Multiprocessing

The bare-metal library provides support for utilizing targets with multiple threads. This includes a simple change in startup file to handle non-zero harts differently. A different stack range is assigned for each hart, and non-zero harts execute a routine in which they loop until a command comes from the main hart.

The MP API (see _lib/include/baremetal/mp.h_) then provides the main hart with functions for instructing the other harts to execute a function. This approach minimizes the changes required for parallelizing an existing program. For example, the mechanism can be easily plugged into the _CoreMark_ benchmark.

The bare-metal library also provides options for hart synchronization, simple barrier and mutex implementations using atomic instructions from the RISC-V A extension (see _lib/include/baremetal/barrier.h_ and _lib/include/baremetal/mutex.h_).

Please examine the relevant demos for usage examples:

- [MP demo](software/mp-demo/README.md)
- [Mutex demo](software/mutex-demo/README.md)

### Interrupts and privilege modes

The bare-metal library provides a rich support for handling RISC-V interrupts, exceptions, and switching privilege modes. However, the software also needs to support the interrupt controllers present in the hardware. To this end, APIs are provided for several devices:

- Core Local INTerruptor (CLINT) or Advanced CLINT (ACLINT), when mapped to memory in a compatible fashion, mainly for generating timer interrupt
- Platform Level Interrupt Controller (PLIC), for handling external interrupts
- Codasip Programmable Interrupt Controller (PIC), also for handling external interrupts

These APIs can be found in _lib/include/baremetal/clint.h_, _lib/include/baremetal/pic.h_ and _lib/include/baremetal/plic.h_. Please see the relevant demos for usage examples:

- [CLINT IPI demo](software/clint-ipi/README.md)
- [CLINT timer demo](software/clint-timer/README.md)
- [CLINT interrupt demo](software/clint-timer-interrupt/README.md)
- [PIC demo](software/pic-interrupts/README.md)
- [PLIC demo](software/plic-interrupts/README.md)
- [PLIC interrupt priority demo](software/plic-priority/README.md)

Interrupts can be either processed by setting custom handlers using low-level interrupt API (see _lib/include/baremetal/interrupt_low.h_), or by utilyzing the provided handlers. Using the low-level API offers more simplicity and flexibility.

Trap handling is also managed dynamically in the provided handlers, the user first needs to initialize the system and can then setup custom handlers during run-time. However, custom handlers can be set directly for a given interrupt, exception or an external interrupt source (see _lib/include/baremetal/interrupt.h_). The provided handler system also accounts for RISC-V privilege modes, and together with functions from the low-level interrupt API allows for interrupt delegation.

Support for privilege mode transfer is provided in _lib/include/baremetal/priv.h_, the user can select what mode to enter and set a separate stack for the new privilege mode. Privilege mode API is therefore tightly integrated with the trap handling API, because upon entering a higher privilege mode the registers need to be saved and the original stack needs to be restored.

Please see the relevant demos for usage examples:

- [ECALL demo](software/ecall-demo/README.md)
- [Exception demo](software/exception-demo/README.md)
- [Interrupt demo](software/interrupts-simple/README.md)
- [Vectored interrupts demo](software/interrupts-vectored/README.md)
- [Privilege ](software/privilege-drop/README.md)
- [Privilege ](software/privilege-interrupts/README.md)
- [Privilege ](software/privilege-interrupts-delegated/README.md)
- [RDTIME demo](software/rdtime/README.md)
- [WFI demo](software/wfi-demo/README.md)

### Peripheral drivers

Codasip's FPGA platforms usually contain a range of hardware peripherals. The Bare-metal library therefore contains simple drivers for these peripherals. APIs for peripherals from Xilinx are provided for GPIO, I2C, SPI, and UART in _lib/include/baremetal/gpio.h_, _lib/include/baremetal/i2c.h_, _lib/include/baremetal/spi.h_, and _lib/include/baremetal/uart.h_, respectively. Please see the relevant demos for usage examples:

- [GPIO demo](software/gpio-demo/README.md)
- [GPIO interrupt demo](software/gpio-interrupt-demo/README.md)
- [I2C demo](software/i2c-demo/README.md)
- [SPI demo](software/spi-demo/README.md)
- [UART demo](software/uart-demo/README.md)

Besides the generic periperals, Codasip's FPGA platforms can also contain more specialized peripherals. For instance, core-specific APIs for the _L31_ core can be used for cache management, or to access Tightly Coupled Memories. The Bare-metal library also provides support for platforms with security peripherals, e.g. a True Random Number Generator (TRNG), or an adapter for the Authenticated Encryption with Associated Data (AEAD) algorithm.

Please see the relevant demos for examples:

- [AEAD](software/aead-demo/README.md)
- [Cache counter demo](software/cache-counter-demo/README.md)
- [Cache info demo](software/cache-info-demo/README.md)
- [Cache write through demo](software/cache-write-through/README.md)
- [TCM demo](software/tcm-demo/README.md)
- [TRNG](software/trng-demo/README.md)


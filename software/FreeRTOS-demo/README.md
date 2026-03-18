# FreeRTOS-demo

Demonstrates use of FreeRTOS with Codasip's BareMetal layer using GPIO to get switch inputs and
flash LEDs, and multiple tasks outputting to the UART via the Mutex protected function `vSendString()`.

`main_blinky.c` is the standard FreeRTOS Demo. Oddly it does not blink LEDs, it sends data from
one task to another outputting information to the UART via `vSendString()`.

`main_buggy.c` is an alternative FreeRTOS demo which can be used to demonstrate CHERI capabilities.
It consists of 3 tasks that overflow buffers. It can be run on both CHERI and non-CHERI platforms
to demonstrate how CHERI can catch and safely handle the buffer overflows whilst data is corrupted
without CHERI enabled.

## Additional Setup

In `Makefile` set:

    RTOS_DIR to point to the FreeRTOS-Kernel directory (the one that has the C files and the
    `include` & `portable` sub-directories in it).

In `FreeRTOS-demo.c` set:

    mainVECTOR_MODE_DIRECT = 0 to use vectored interrupt mode (and interrupt vector table vector.S)
    mainVECTOR_MODE_DIRECT = 1 to use direct interrupt mode

    UART_LOCAL_USE = 0 to use SYS UART (via printf)
    UART_LOCAL_USE = 1 to setup the UART locally in this file
        UART_LOCAL_IRQ_ENABLE = 0 do not use IRQ for the local UART (blocking)
        UART_LOCAL_IRQ_ENABLE = 1 do use IRQ for the local UART

    EXCEPTION_HANDLER_LOCAL_USE = 0 Use BareMetal Exception Handler
    EXCEPTION_HANDLER_LOCAL_USE = 1 Handle exceptions locally in this file

## Building

To build this demo you need Codasip's port of `FreeRTOS-Kernel` repo, clone it from:

	https://github.com/Codasip/FreeRTOS-Kernel.git

For example, to clone it with all submodules into a subdirectory:

    mkdir -p repos
    cd repos
    git clone https://github.com/Codasip/FreeRTOS-Kernel.git --recurse-submodules
    cd ..

This demo has been tested with `FreeRTOS V11.2.0`.

Set up the environment variables to point to the repo:

    export RTOS_DIR=$(realpath repos/FreeRTOS-Kernel)

Set up the environment variables to build your selected demo:

    export FREERTOS_DEMO_APP=blinky
        or
    export FREERTOS_DEMO_APP=buggy

    Note: By default FREERTOS_DEMO_APP is set to blinky and does not need setting if this is your
    desired application.

After configuring `baremetal-examples/config.mk` and adding environment variable `SDK_PREFIX`:

    cd baremetal-examples/software/FreeRTOS-demo
    make

## Sample Output

    ======================================================
    FreeRTOS Version V11.2.0+
    BareMetal Examples Version 1.0.0

    FreeRTOS multi-task example running
    3 tasks outputting text with 1 also flashing LEDs.
    ======================================================

    This (Fx) task is the BareMetal GPIO demo in a FreeRTOS task
    with an additional counting number output.
    Feel free to flip switches 1-3, and observe blinking LEDs.

    The other (Tx/Rx) tasks are the standard FreeRTOS Demo app (main_blinky.c)

    0: Fx: 1
    0: Tx: Transfer1
    0: Fx: 2
    0: Fx: 3
    0: Fx: 4
    0: Fx: 5
    0: Rx: Blink1
    0: Tx: Transfer2
    0: Fx: 6
    0: Fx: 7
    0: Fx: 8
    0: Fx: 9
    0: Rx: Blink2
    0: Tx: Transfer1
    0: Fx: 10
    0: Fx: 11
    0: Fx: 12
    0: Fx: 13
    0: Rx: Blink1
    0: Tx: Transfer2
    0: Fx: 14
    0: Fx: 15
    0: Fx: 16
    0: Fx: 17

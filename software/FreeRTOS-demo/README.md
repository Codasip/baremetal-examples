# FreeRTOS-demo

Demonstrates use of FreeRTOS with Codasip's BareMetal layer using GPIO to get switch inputs and
flash LEDs, and multiple tasks outputting to the UART via the Mutex protected function `vSendString()`.

`main_blinky.c` is the standard FreeRTOS Demo. Oddly it does not blink LEDs, it sends data from
one task to another outputting information to the UART via `vSendString()`.

## Additional Setup

In `Makefile` set:

    RTOS_DIR to point to the FreeRTOS directory (the one that has Source and Demo sub-directories)

In `FreeRTOS-demo.c` set:

    mainVECTOR_MODE_DIRECT = 0 to use vectored interrupt mode (and interrupt vector table vector.S)
    mainVECTOR_MODE_DIRECT = 1 to use direct interrupt mode

    UART_LOCAL_USE = 0 to use SYS UART (via printf)
    UART_LOCAL_USE = 1 to setup the UART locally in this file
        UART_LOCAL_IRQ_ENABLE = 0 do not use IRQ for the local UART (blocking)
        UART_LOCAL_IRQ_ENABLE = 1 do use IRQ for the local UART

    EXCEPTION_HANDLER_LOCAL_USE = 0 Handle exceptions locally in this file
    EXCEPTION_HANDLER_LOCAL_USE = 1 Use BareMetal Exception Handler

## Building

To build this demo you need the `FreeRTOS` repo, clone it from:

	https://github.com/FreeRTOS/FreeRTOS.git

For example, to clone and recursively get all the sub-modules, do:

    git clone https://github.com/FreeRTOS/FreeRTOS.git --recurse-submodules

This demo has been tested with `FreeRTOS V11.1.0`.

In the directory FreeRTOS/Demo:

    mkdir RISC-V_Codasip_GCC

In the sub-directory `RISC-V_Codasip_GCC`:

    git clone baremetal-examples

Alternatively change `RTOS_DIR` to point to FreeRTOS elsewhere.

    RTOS_DIR ?= ../../../../../../FreeRTOS

After configuring `baremetal-examples/config.mk` and adding environment variable `SDK_PREFIX`:

    cd baremetal-examples/software/FreeRTOS-demo
    make

## Sample Output

    ======================================================
    FreeRTOS Version V11.1.0+
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

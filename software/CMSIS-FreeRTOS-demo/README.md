# CMSIS-FreeRTOS

Demonstrates use of CMSIS-RTOS2 wrapper and FreeRTOS with Codasip's BareMetal layer using GPIO
to get switch inputs and flash LEDs, and multiple tasks outputting to the UART via the Mutex
protected function `vSendString()`.

`MsgQueueDemo.c` is based on `main_blinky.c`, the standard FreeRTOS Demo. It sends data from
one task to another outputting information to the UART via `vSendString()`.

## Additional Setup

In `Makefile` set:

    RTOS_DIR to point to the FreeRTOS-Kernel directory (the one that has the C files and the
    `include` & `portable` sub-directories in it) containing a clone of the following repo:
    https://github.com/Codasip/FreeRTOS-Kernel.git

    CMSIS_DIR to point to a CMSIS directory containing clones of the following repos:
    https://github.com/Codasip/CMSIS_6.git
    https://github.com/ARM-software/CMSIS-FreeRTOS.git

In `CMSIS-FreeRTOS-demo.h` set:

    mainVECTOR_MODE_DIRECT = 0 to use vectored interrupt mode (and interrupt vector table vector.S)
    mainVECTOR_MODE_DIRECT = 1 to use direct interrupt mode

    UART_LOCAL_USE = 0 to use SYS UART (via printf)
    UART_LOCAL_USE = 1 to setup the UART locally in this file
        UART_LOCAL_IRQ_ENABLE = 0 do not use IRQ for the local UART (blocking)
        UART_LOCAL_IRQ_ENABLE = 1 do use IRQ for the local UART

    EXCEPTION_HANDLER_LOCAL_USE = 0 Use BareMetal Exception Handler
    EXCEPTION_HANDLER_LOCAL_USE = 1 Handle exceptions locally in this file

## Building

To build this demo you need the following repos, clone them from:

    https://github.com/Codasip/FreeRTOS-Kernel.git
    https://github.com/Codasip/CMSIS_6.git
    https://github.com/ARM-software/CMSIS-FreeRTOS.git

For example, to clone them with all submodules into a subdirectory:

    mkdir -p repos
    cd repos
    git clone https://github.com/Codasip/FreeRTOS-Kernel.git --recurse-submodules
    git clone https://github.com/Codasip/CMSIS_6.git --recurse-submodules
    git clone https://github.com/ARM-software/CMSIS-FreeRTOS.git --recurse-submodules
    cd ..

This demo has been tested with `FreeRTOS V11.2.0`.

Set up the environment variables to point to the repos:

    export RTOS_DIR=$(realpath repos/FreeRTOS-Kernel)
    export CMSIS_6_DIR=$(realpath repos/CMSIS_6)
    export CMSIS_RTX_DIR=$(realpath repos/CMSIS-RTX)

After configuring `baremetal-examples/config.mk` and adding environment variable `SDK_PREFIX`:

    cd baremetal-examples/softare/CMSIS-FreeRTOS-demo
    make

## Sample Output

======================================================
FreeRTOS Version V11.2.0
BareMetal Examples Version 1.1.1

CMSIS-RTOS2-FreeRTOS multi-task example running

3 tasks outputting text with 1 also flashing LEDs.
======================================================

This (Fx) task is the BareMetal GPIO demo in a RTX/FreeRTOS task
with an additional counting number output.
Feel free to flip switches 1-3, and observe blinking LEDs.

The other (Tx/Rx) tasks demonstrate a message queue.

0: Fx: 1
Sending A
Received A
0: Fx: 2
0: Fx: 3
0: Fx: 4
0: Fx: 5
Sending B
Received B
0: Fx: 6
0: Fx: 7
0: Fx: 8
0: Fx: 9
Sending C
Received C
0: Fx: 10
0: Fx: 11
0: Fx: 12
0: Fx: 13
Sending D
Received D
0: Fx: 14
0: Fx: 15
0: Fx: 16
0: Fx: 17
Sending E
Received E
0: Fx: 18
0: Fx: 19
0: Fx: 20
0: Fx: 21
Sending F
Received F
0: Fx: 22
0: Fx: 23
0: Fx: 24
0: Fx: 25
Sending G
Received G

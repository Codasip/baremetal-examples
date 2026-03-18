# CMSIS-RTX

Demonstrates use of Codasip's CMSIS_6 & CMSIS-RTX RISC-V Ports with Codasip's BareMetal layer using
GPIO to get switch inputs and flash LEDs, and multiple tasks outputting to the UART via the Mutex
protected function `vSendString()`.

`MsgQueueDemo.c` is based on `main_blinky.c`, the standard FreeRTOS Demo. It sends data from
one task to another outputting information to the UART via `vSendString()`.

This demo is a copy of `CMSIS-FreeRTOS-demo` and has been modifed to Codasip CMSIS_6 & RTX Ports
and uses the same CMSIS-RTOS2 API.

## Additional Setup

In `Makefile` or your environment set:

    `CMSIS_DIR` to point to a CMSIS directory containing clones of the following repos:
    https://github.com/Codasip/CMSIS_6.git
    https://github.com/Codasip/CMSIS-RTX.git

In `CMSIS-RTX-demo.c` set:

    UART_LOCAL_USE = 0 to use SYS UART (via printf) in `lib/src/uart.c`
    UART_LOCAL_USE = 1 to setup the UART locally in this file and `rtx_uart.c` (a modified copy of `lib/src/uart.c`)

        UART_LOCAL_IRQ_ENABLE = 0 do not use interrupts for the local UART (i.e. blocking calls)
        UART_LOCAL_IRQ_ENABLE = 1 use interrupts for the local UART

            UART_LOCAL_IRQ_VECTORED = 0 use the vectored ISR uart_interrupt_handler()
            UART_LOCAL_IRQ_VECTORED = 1 use the non-vectored ISR handler rtx_risc_v_interrupt_handler()

## Building

To build this demo you need the following repos, clone them from:

    https://github.com/Codasip/CMSIS_6.git
    https://github.com/Codasip/CMSIS-RTX.git

For example, to clone them with all submodules into a subdirectory:

    mkdir -p repos
    cd repos
    git clone https://github.com/Codasip/CMSIS_6.git --recurse-submodules
    git clone https://github.com/Codasip/CMSIS-RTX.git --recurse-submodules
    cd ..

This demo has been tested with `RTX V5.9.0`.

Set up the environment variables to point to the repos:

    export CMSIS_6_DIR=$(realpath repos/CMSIS_6)
    export CMSIS_RTX_DIR=$(realpath repos/CMSIS-RTX)

After configuring `baremetal-examples/config.mk` and adding environment variable `SDK_PREFIX`:

    cd baremetal-examples/software/CMSIS-RTX-demo
    make

## Sample Output

======================================================
Codasip's RISC-V port of RTX Version RTX V5.9.0

BareMetal Examples Version 1.1.4

CMSIS-RTX multi-task example running

3 tasks outputting text with 1 also flashing LEDs.
Sending A
======================================================

Received A
This (Fx) task is the BareMetal GPIO demo in a RTX task
with an additional counting number output.
Feel free to flip switches 1-3, and observe blinking LEDs.

The other (Tx/Rx) tasks demonstrate a message queue.

0: Fx: 1
0: Fx: 2
0: Fx: 3
0: Fx: 4
Sending B
Received B
0: Fx: 5
0: Fx: 6
0: Fx: 7
0: Fx: 8
Sending C
Received C
0: Fx: 9
0: Fx: 10
0: Fx: 11
0: Fx: 12
Sending D
Received D

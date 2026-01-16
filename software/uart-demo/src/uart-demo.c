/* Copyright 2023-2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/bm_cheri.h"
#include "baremetal/common.h"

#include <baremetal/interrupt.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <baremetal/uart.h>
#include <stdio.h>
#include <stdlib.h>

// Configuration of the demo
#define USE_IRQ 1

static bm_uart_t *uart;

/**
 * \brief Helper function for printing out a whole string over UART
 *
 * \param str String to print
 */
static inline void write_line(const char *str)
{
    while (*str != 0)
    {
        bm_uart_transmit_byte(uart, *str++);
    }
}

/**
 * \brief Interrupt handler for UART
 */
#if USE_IRQ
static void uart_interrupt_handler(bm_register_file_t *stacked_regs)
{
    (void)stacked_regs; // unused

    bm_uart_handle_irq(uart);
}
#endif

int main(void)
{
    puts("Welcome to the UART demo!\n");
    puts("Connect to serial terminal with 115200 baud rate and 8-N-1 settings, to interract with "
         "the program.");

    /* NOTE: Currently, when using Codasip Bakewell SDK the Bare Metal build uses the SDK's Lib
     * Gloss and startup files (instead of the Bare Metal UART and crt0.S).
     * This demo also uses bm_uart driver for the same UART. So for this demo to work correctly
     * do no use any stdio function calls after bm_uart_init() (instead use the bm_uart_*() calls only).
     */

    uart = (bm_uart_t *)target_peripheral_get(BM_PERIPHERAL_UART_CONSOLE);

    bm_uart_config_t config = {.baud_rate   = 115200,
                               .data_format = BM_UART_DATA_BITS_8,
                               .parity      = BM_UART_PARITY_NONE,
                               .stop        = BM_UART_STOP_BITS_1,
                               .use_irq     = USE_IRQ};
    bm_uart_init(uart, &config);

#if USE_IRQ
    bm_ext_irq_set_handler(uart->ext_irq_id, uart_interrupt_handler);
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MEIP);
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);
#endif

    write_line("\r\nPress any key.\r\n");

    unsigned i          = 0;
    unsigned prev_print = bm_get_time_ms();

    while (1)
    {
        int c = bm_uart_receive_byte(uart);

        // Quit
        if (c == 'q')
        {
            break;
        }

        // Acknowledge received character
        if (c != -1)
        {
            i          = 0;
            prev_print = bm_get_time_ms();

            write_line("Pressed '");
            bm_uart_transmit_byte(uart, c);
            write_line("', press 'q' to exit the loop\r\n");
            continue;
        }

        if ((prev_print + 100) < bm_get_time_ms())
        {
            write_line("Waiting for input");

            for (unsigned j = 0; j < 5; ++j)
            {
                bm_uart_transmit_byte(uart, j < i ? '.' : ' ');
            }

            bm_uart_transmit_byte(uart, '\r');

            i          = (i + 1) % 5;
            prev_print = bm_get_time_ms();
        }
    }

    write_line("\r\nBye.\r\n");
    bm_uart_flush(uart);
    return EXIT_SUCCESS;
}

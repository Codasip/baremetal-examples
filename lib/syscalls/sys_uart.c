/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/common.h"
#include "baremetal/platform.h"
#include "baremetal/uart.h"
#include "baremetal/verbose.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define BM_SYS_UART_BAUD 115200

static volatile bool bm_sys_init_done = false;
static bm_uart_t    *sys_uart;

/**
 * \brief Initialize the UART peripheral to be used for syscalls
 *
 * This initialization only needs to be done once, if the peripheral's configuration is not changed.
 */
static inline void bm_sys_uart_init(void)
{
    if (bm_sys_init_done)
    {
        return;
    }

    sys_uart = (bm_uart_t *)target_peripheral_get(BM_PERIPHERAL_UART_CONSOLE);

    bm_uart_config_t cfg = {.baud_rate   = BM_SYS_UART_BAUD,
                            .data_format = BM_UART_DATA_BITS_8,
                            .parity      = BM_UART_PARITY_NONE,
                            .stop        = BM_UART_STOP_BITS_1,
                            .use_irq     = false};
    bm_uart_init(sys_uart, &cfg);

    bm_sys_init_done = true;
}

/**
 * \brief Implementation of the write syscall using UART peripheral
 *
 * Assumes a UART peripheral is available on the target,
 * and that no code outside this file is using the peripheral.
 *
 * \param fd File to write to
 * \param ptr Pointer to buffer with characters to write
 * \param len Number of characters to write
 *
 * \return Number of characters written on success, -1 on error
 */
_READ_WRITE_RETURN_TYPE USED _write(int fd, const void *ptr, size_t len)
{
    if (fd != STDOUT_FILENO && fd != STDERR_FILENO)
    {
        errno = EBADF;
        return -1;
    }

    bm_sys_uart_init();

    // Transmit the chars over UART
    for (unsigned i = 0; i < len; ++i)
    {
        if (((char *)ptr)[i] == '\n')
        {
            bm_uart_transmit_byte(sys_uart, '\r');
        }
        bm_uart_transmit_byte(sys_uart, ((char *)ptr)[i]);
    }
    bm_uart_flush(sys_uart);

    return len;
}

/**
 * \brief Implementation of the read syscall using UART peripheral
 *
 * Assumes a UART peripheral is available on the target,
 * and that no code outside this file is using the peripheral.
 *
 * \param fd File to read from
 * \param ptr Pointer to buffer for read characters
 * \param len Number of characters to read
 *
 * \return Number of characters read on success, -1 on error
 */
_READ_WRITE_RETURN_TYPE USED _read(int fd, void *ptr, size_t len)
{
    if (fd != STDIN_FILENO)
    {
        errno = EBADF;
        return -1;
    }

    bm_sys_uart_init();

    size_t bytes_read = 0;

    // Receive the chars over UART
    while (bytes_read < len)
    {
        int c = bm_uart_receive_byte(sys_uart);

        if (c == -1 && bytes_read == 0)
        {
            // Blocks if no characters are available
            continue;
        }

        if (c == -1)
        {
            break;
        }

        ((char *)ptr)[bytes_read++] = c;
    }

    return bytes_read;
}

void USED _exit(int ret)
{
    if (ret)
    {
        bm_info("Exited with an error.");
    }
    else
    {
        bm_info("Exited normally.");
    }

    while (1)
        ;
}

_READ_WRITE_RETURN_TYPE USED write(int fd UNUSED, const void *ptr UNUSED, size_t len UNUSED)
    __attribute__((alias("_write")));
_READ_WRITE_RETURN_TYPE USED read(int fd UNUSED, void *ptr UNUSED, size_t len UNUSED)
    __attribute__((alias("_read")));

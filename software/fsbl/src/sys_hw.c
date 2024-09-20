/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "fatfs/ff.h"

#include <baremetal/common.h>
#include <baremetal/platform.h>
#include <baremetal/uart.h>
#include <baremetal/verbose.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

static FATFS         fatfs;
static FIL           fp[FOPEN_MAX - 3];
static uint32_t      open_fd      = 0;
static volatile bool sd_init_done = false;

int USED _open(const char *file, int flags, ...)
{
    if (!sd_init_done)
    {
        FRESULT result = f_mount(&fatfs, "", 1);
        if (result == FR_NOT_READY)
        {
            // Retry to increase reliability in case of missing SD_nRESET signal
            result = f_mount(&fatfs, "", 1);
        }
        if (result != FR_OK)
        {
            errno = ENODEV;
            return -1;
        }
        sd_init_done = true;
    }

    unsigned fd = 3;
    while (open_fd & (1 << fd))
    {
        if (++fd >= FOPEN_MAX)
        {
            errno = EMFILE;
            return -1;
        }
    }

    unsigned mode;
    switch (flags)
    {
        // "r"
        case O_RDONLY:
            mode = FA_READ;
            break;
        default:
            errno = ENOTSUP;
            return -1;
    }

    FRESULT result = f_open(&fp[fd - 3], file, mode);
    if (result)
    {
        errno = ENOENT;
        return -1;
    }

    open_fd |= (1 << fd);
    return fd;
}

int USED _close(int fd)
{
    if (!(open_fd & (1 << fd)))
    {
        errno = EBADF;
        return -1;
    }

    f_close(&fp[fd - 3]);
    open_fd &= ~(1 << fd);

    if (!open_fd)
    {
        f_unmount("");
        sd_init_done = false;
    }

    return 0;
}

off_t USED _lseek(int fd, off_t offset, int whence)
{
    if (!(open_fd & (1 << fd)))
    {
        errno = EBADF;
        return -1;
    }

    FRESULT result = -1;
    switch (whence)
    {
        case SEEK_SET:
            result = f_lseek(&fp[fd - 3], offset);
            break;
        case SEEK_CUR:
            result = f_lseek(&fp[fd - 3], f_tell(&fp[fd - 3]) + offset);
            break;
        case SEEK_END:
            result = f_lseek(&fp[fd - 3], f_size(&fp[fd - 3]));
            break;
    }

    if (result)
    {
        return -1;
    }

    return f_tell(&fp[fd - 3]);
}

/**
 * \brief Implementation of the read syscall using SD peripheral
 *
 * \param fd File to read from
 * \param ptr Pointer to buffer for read characters
 * \param len Number of characters to read
 *
 * \return Number of characters read on success, -1 on error
 */
_READ_WRITE_RETURN_TYPE USED _read(int fd, void *ptr, size_t len)
{
    size_t bytes_read = 0;

    if (!(open_fd & (1 << fd)))
    {
        errno = EBADF;
        return -1;
    }

    if (f_read(&fp[fd - 3], ptr, len, (UINT *)&bytes_read))
    {
        errno = EIO;
        return -1;
    }

    return bytes_read;
}

#define BM_SYS_UART_BAUD 115200

/**
 * \brief Implementation of the write syscall using UART peripheral
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

    bm_uart_t *sys_uart = (bm_uart_t *)target_peripheral_get(BM_PERIPHERAL_UART_CONSOLE);

    static bool uart_init_done = false;
    if (!uart_init_done)
    {
        bm_uart_config_t cfg = {.baud_rate   = BM_SYS_UART_BAUD,
                                .data_format = BM_UART_DATA_BITS_8,
                                .parity      = BM_UART_PARITY_NONE,
                                .stop        = BM_UART_STOP_BITS_1,
                                .use_irq     = false};

        bm_uart_init(sys_uart, &cfg);
        uart_init_done = true;
    }

    /* Transmit the chars over UART */
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
int USED   close(int file UNUSED) __attribute__((alias("_close")));
off_t USED lseek(int file UNUSED, off_t ptr UNUSED, int dir UNUSED)
    __attribute__((alias("_lseek")));
int USED open(const char *file UNUSED, int flags UNUSED, ...) __attribute__((alias("_open")));

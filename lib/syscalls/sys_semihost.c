/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/common.h"
#include "baremetal/verbose.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define SEMIHOSTING_SYS_OPEN  0x01
#define SEMIHOSTING_SYS_CLOSE 0x02
#define SEMIHOSTING_SYS_WRITE 0x05
#define SEMIHOSTING_SYS_READ  0x06
#define SEMIHOSTING_SYS_SEEK  0x0A
#define SEMIHOSTING_SYS_FLEN  0x0C
#define SEMIHOSTING_SYS_EXIT  0x18

struct fdentry {
    int   handle;
    off_t pos;
};

static struct fdentry fp[FOPEN_MAX];
static uint32_t       open_fd = 0;

int syscall_semihosting(xlen_t op, xlen_t param)
{
    int ret;
    __asm__ volatile("mv a0, %1\n"
                     "mv a1, %2\n"
                     ".option push\n"
                     ".option norvc\n"
                     "slli zero, zero, 0x1f\n"
                     "ebreak\n"
                     "srai zero, zero, 0x7\n"
                     ".option pop\n"
                     "mv %0, a0"
                     : "=r"(ret)
                     : "r"(op), "r"(param)
                     : "a0", "a1", "memory");
    return ret;
}

_READ_WRITE_RETURN_TYPE USED _write(int fd, const void *ptr, size_t len)
{
    struct params {
        xlen_t file_desc;
        xlen_t ptr;
        xlen_t len;
    } my_params;

    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        my_params.file_desc = fd;
    }
    else if (open_fd & (1 << fd))
    {
        my_params.file_desc = fp[fd].handle;
    }
    else
    {
        errno = EBADF;
        return -1;
    }

    my_params.len = len;
    my_params.ptr = (xlen_t)ptr;
    xlen_t addr   = (xlen_t)&my_params;

    int ret = syscall_semihosting(SEMIHOSTING_SYS_WRITE, addr);

    return (len - ret);
}

_READ_WRITE_RETURN_TYPE USED _read(int fd, void *ptr, size_t len)
{
    struct params {
        xlen_t file_desc;
        xlen_t ptr;
        xlen_t len;
    } my_params;

    if (fd == STDIN_FILENO)
    {
        my_params.file_desc = fd;
    }
    else if (open_fd & (1 << fd))
    {
        my_params.file_desc = fp[fd].handle;
    }
    else
    {
        errno = EBADF;
        return -1;
    }

    my_params.len = len;
    my_params.ptr = (xlen_t)ptr;
    xlen_t addr   = (xlen_t)&my_params;

    int ret = syscall_semihosting(SEMIHOSTING_SYS_READ, addr);

    return (len - ret);
}

#define SEMIHOST_MODE_R     0
#define SEMIHOST_MODE_RPLUS 2
#define SEMIHOST_MODE_W     4
#define SEMIHOST_MODE_WPLUS 6
#define SEMIHOST_MODE_A     8
#define SEMIHOST_MODE_APLUS 10

int USED _open(const char *file, int flags, ...)
{
    unsigned fd = 3;
    while (open_fd & (1 << fd))
    {
        if (++fd >= FOPEN_MAX)
        {
            errno = EMFILE;
            return -1;
        }
    }

    int mode;
    if ((flags & (O_RDONLY | O_WRONLY | O_RDWR)) == O_RDONLY)
        mode = SEMIHOST_MODE_R;
    else if ((flags & (O_WRONLY | O_CREAT | O_TRUNC)) == (O_WRONLY | O_CREAT | O_TRUNC))
        mode = SEMIHOST_MODE_W;
    else if ((flags & (O_WRONLY | O_CREAT | O_APPEND)) == (O_WRONLY | O_CREAT | O_APPEND))
        mode = SEMIHOST_MODE_A;
    else if ((flags & (O_RDWR | O_CREAT | O_TRUNC)) == (O_RDWR | O_CREAT | O_TRUNC))
        mode = SEMIHOST_MODE_WPLUS;
    else if ((flags & (O_RDWR | O_CREAT | O_APPEND)) == (O_RDWR | O_CREAT | O_APPEND))
        mode = SEMIHOST_MODE_APLUS;
    else if (flags & O_RDWR)
        mode = SEMIHOST_MODE_RPLUS;
    else
    {
        errno = EINVAL;
        return -1;
    }

    struct params {
        xlen_t name;
        xlen_t mode;
        xlen_t len;
    } my_params;

    my_params.name = (xlen_t)file;
    my_params.mode = mode;
    my_params.len  = strlen(file);
    xlen_t addr    = (xlen_t)&my_params;

    int ret = syscall_semihosting(SEMIHOSTING_SYS_OPEN, addr);

    if (ret == -1)
    {
        return ret;
    }

    open_fd |= (1 << fd);
    fp[fd].handle = ret;
    fp[fd].pos    = 0;

    return fd;
}

int USED _close(int fd)
{
    if (!(open_fd & (1 << fd)))
    {
        errno = EBADF;
        return -1;
    }

    struct params {
        xlen_t file_desc;
    } my_params;

    my_params.file_desc = fd;
    xlen_t addr         = (xlen_t)&my_params;

    syscall_semihosting(SEMIHOSTING_SYS_CLOSE, addr);

    open_fd &= ~(1 << fd);

    return 0;
}

off_t USED _lseek(int fd, off_t offset, int whence)
{
    if (!(open_fd & (1 << fd)))
    {
        errno = EBADF;
        return -1;
    }

    struct params {
        xlen_t file_desc;
        xlen_t pos;
    } my_params;

    my_params.file_desc = fp[fd].handle;
    my_params.pos       = 0;
    xlen_t addr         = (xlen_t)&my_params;

    int ret;
    switch (whence)
    {
        case SEEK_SET:
            my_params.pos = offset;
            break;
        case SEEK_CUR:
            my_params.pos = fp[fd].pos + offset;
            break;
        case SEEK_END:

            ret = syscall_semihosting(SEMIHOSTING_SYS_FLEN, addr);
            if (ret == -1)
                return -1;

            my_params.pos = ret + offset;
            break;
        default:
            errno = EINVAL;
            return -1;
    }

    ret = syscall_semihosting(SEMIHOSTING_SYS_SEEK, addr);

    if (ret == 0)
    {
        fp[fd].pos = my_params.pos;
        return fp[fd].pos;
    }

    return ret;
}

#define ADP_Stopped_ApplicationExit     0x20026
#define ADP_Stopped_RunTimeErrorUnknown 0x20023

void USED _exit(int ret)
{
    if (ret == 0)
    {
        bm_info("Exited normally.");
    }
    else
    {
        bm_info("Exited with an error.");
    }

    xlen_t addr;
#if __riscv_xlen == 32
    addr = (ret == 0) ? (xlen_t)ADP_Stopped_ApplicationExit : (xlen_t)ADP_Stopped_RunTimeErrorUnknown;
#else
    struct params {
        xlen_t magic;
        xlen_t return_code;
    } my_params;

    my_params.return_code = ret;
    my_params.magic       = ADP_Stopped_ApplicationExit;

    addr = (xlen_t)&my_params;
#endif

    syscall_semihosting(SEMIHOSTING_SYS_EXIT, addr);

    while (1)
        ;
}

_READ_WRITE_RETURN_TYPE USED write(int fd, const void *ptr, size_t len)
    __attribute__((alias("_write")));
_READ_WRITE_RETURN_TYPE USED read(int fd, void *ptr, size_t len) __attribute__((alias("_read")));
int USED                     close(int fd) __attribute__((alias("_close")));
int USED                     open(const char *file, int flags, ...) __attribute__((alias("_open")));
off_t USED lseek(int fd, off_t offset, int whence) __attribute__((alias("_lseek")));

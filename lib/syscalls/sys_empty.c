/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/common.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

_READ_WRITE_RETURN_TYPE WEAK USED _write(int fd UNUSED, const void *ptr UNUSED, size_t len UNUSED)
{
    errno = -ENOSYS;
    return -1;
}

_READ_WRITE_RETURN_TYPE WEAK USED _read(int fd UNUSED, void *ptr UNUSED, size_t len UNUSED)
{
    errno = -ENOSYS;
    return -1;
}

int WEAK USED _isatty(int file UNUSED)
{
    errno = -ENOSYS;
    return -1;
}

int WEAK USED _fstat(int file UNUSED, struct stat *st UNUSED)
{
    errno = -ENOSYS;
    return -1;
}

int WEAK USED _close(int file UNUSED)
{
    errno = ENOSYS;
    return -1;
}

off_t WEAK USED _lseek(int file UNUSED, off_t ptr UNUSED, int dir UNUSED)
{
    errno = ENOSYS;
    return -1;
}

void *WEAK USED _sbrk(intptr_t nbytes UNUSED)
{
    errno = ENOSYS;
    return (void *)-1;
}

int WEAK USED _open(const char *file UNUSED, int flags UNUSED, ...)
{
    errno = ENOSYS;
    return -1;
}

_READ_WRITE_RETURN_TYPE WEAK USED write(int fd UNUSED, const void *ptr UNUSED, size_t len UNUSED)
    __attribute__((alias("_write")));
_READ_WRITE_RETURN_TYPE WEAK USED read(int fd UNUSED, void *ptr UNUSED, size_t len UNUSED)
    __attribute__((alias("_read")));
int WEAK USED   isatty(int file UNUSED) __attribute__((alias("_isatty")));
int WEAK USED   fstat(int file UNUSED, struct stat *st UNUSED) __attribute__((alias("_fstat")));
int WEAK USED   close(int file UNUSED) __attribute__((alias("_close")));
off_t WEAK USED lseek(int file UNUSED, off_t ptr UNUSED, int dir UNUSED)
    __attribute__((alias("_lseek")));
void *WEAK USED sbrk(intptr_t nbytes UNUSED) __attribute__((alias("_sbrk")));
int WEAK USED open(const char *file UNUSED, int flags UNUSED, ...) __attribute__((alias("_open")));

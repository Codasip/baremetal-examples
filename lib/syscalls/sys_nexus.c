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

#define CODASIP_SYSCALL_OP_READ  1
#define CODASIP_SYSCALL_OP_WRITE 2
#define CODASIP_SYSCALL_OP_OPEN  3
#define CODASIP_SYSCALL_OP_CLOSE 4
#define CODASIP_SYSCALL_OP_LSEEK 5

// defined in the startup
extern volatile unsigned int codasip_syscall;

typedef struct {
    int          op;
    volatile int ret;
    volatile int errno_set;
    volatile int errno_value;
    union {
        int file_desc;
        int pathname;
        int fildes;
    };
    union {
        int ptr;
        int length;
        int offset;
    };
    union {
        int len;
        int flags;
        int whence;
    };
} nexus_param_t;

#define CODASIP_SYSCALL_EXIT_PARAM (nexus_param_t *)1

static int nexus_syscall(nexus_param_t *param, int ret)
{
    __asm__ volatile(".option push\n"
                     ".option norvc\n"
                     "mv a0, %2\n"
                     "nop\n"
                     "sw %0, 0 (%1)\n"
                     "ebreak\n"
                     ".option pop\n" ::"r"(param),
                     "r"(&codasip_syscall),
                     "r"(ret)
                     : "a0");

    if (param == CODASIP_SYSCALL_EXIT_PARAM)
    {
        return 0;
    }

    if (param->errno_set)
    {
        errno = param->errno_value;
    }
    return param->ret;
}

_READ_WRITE_RETURN_TYPE USED _write(int file, const void *ptr, size_t len)
{
    nexus_param_t params = {.op        = CODASIP_SYSCALL_OP_WRITE,
                            .file_desc = file,
                            .len       = len,
                            .ptr       = (xlen_t)ptr};

    return nexus_syscall(&params, 0);
}

_READ_WRITE_RETURN_TYPE USED _read(int file, const void *ptr, size_t len)
{
    nexus_param_t params = {.op        = CODASIP_SYSCALL_OP_READ,
                            .file_desc = file,
                            .len       = len,
                            .ptr       = (xlen_t)ptr};

    return nexus_syscall(&params, 0);
}

int USED _close(int file)
{
    nexus_param_t params = {.op = CODASIP_SYSCALL_OP_CLOSE, .file_desc = file};

    return nexus_syscall(&params, 0);
}

int USED _open(const char *pathname, int flags, ...)
{
    nexus_param_t params = {.op       = CODASIP_SYSCALL_OP_OPEN,
                            .pathname = (xlen_t)pathname,
                            .length   = (int)strlen(pathname),
                            .flags    = flags};

    return nexus_syscall(&params, 0);
}

off_t USED _lseek(int fildes, off_t offset, int whence)
{
    nexus_param_t params = {.op     = CODASIP_SYSCALL_OP_LSEEK,
                            .fildes = fildes,
                            .offset = offset,
                            .whence = whence};

    return nexus_syscall(&params, 0);
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

    nexus_syscall(CODASIP_SYSCALL_EXIT_PARAM, ret);

    while (1)
        ;
}

_READ_WRITE_RETURN_TYPE USED write(int file, const void *ptr, size_t len)
    __attribute__((alias("_write")));
_READ_WRITE_RETURN_TYPE USED read(int file, void *ptr, size_t len) __attribute__((alias("_read")));
int USED                     close(int file) __attribute__((alias("_close")));
int USED   open(const char *pathname, int flags, ...) __attribute__((alias("_open")));
off_t USED lseek(int fildes, off_t offset, int whence) __attribute__((alias("_lseek")));

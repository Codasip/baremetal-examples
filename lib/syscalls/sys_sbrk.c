/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/common.h"
#include "baremetal/verbose.h"

#include <errno.h>
#include <stdint.h>

extern char  _heap[], _heap_end[];
static char *heap_ptr;

/**
 * \brief Implementation of the sbrk syscall
 *
 * \param nbytes Number of bytes to allocate
 *
 * \return Pointer to the allocated memory, -1 on error
 */
void *USED _sbrk(intptr_t nbytes)
{
    if (!heap_ptr)
    {
        heap_ptr = (char *)_heap;
    }

    if ((heap_ptr + nbytes) > _heap_end)
    {
        bm_warn("Failed to allocate space on heap.");

        errno = ENOMEM;
        return (void *)-1;
    }

    char *base = heap_ptr;
    heap_ptr += nbytes;

    return base;
}

void *USED sbrk(intptr_t nbytes) __attribute__((alias("_sbrk")));

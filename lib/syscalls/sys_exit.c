/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/common.h"
#include "baremetal/verbose.h"

void USED WEAK NORETURN _exit(int exitcode)
{
    if (exitcode != 0)
    {
        bm_info("Exited with an error (%d)", exitcode);
    }
    else
    {
        bm_info("Exited normally.");
    }

    // Call environment spcific exit function, which shall not return.
    if (env_do_exit)
    {
        env_do_exit(exitcode);
    }

    bm_warn("No way to exit, running infinite loop.");

    // Every environment should implement env_do_exit() and put the core into a
    // low power state at least, e.g. by looping over a WFI. If we arrive here,
    // there is nothing we can do. Running a busy loop to keep the core up and
    // allow attaching a debugger seems the best option.

    while (1)
        ;
}

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <stdlib.h>
#include <tiny_printf/printf.h>

#define BM_MSG(cause, msg) printf(cause ": %s: " msg "\n", __func__);

/** \brief Report a fatal error to the user and end the execution */
#define bm_fatal(msg)         \
    do                        \
    {                         \
        BM_MSG("Fatal", msg); \
        exit(1);              \
    } while (0)

/** \brief Report an error to the user */
#define bm_error(msg) BM_MSG("Error", msg)

/** \brief Report a warning to the user */
#define bm_warn(msg) BM_MSG("Warning", msg)

/** \brief Report an info message to the user */
#define bm_info(msg) BM_MSG("Info", msg)

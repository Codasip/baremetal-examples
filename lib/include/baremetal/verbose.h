/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <stdarg.h>
#include <stdlib.h>
#include <tiny_printf/printf.h>

void bm_message(const char *cause, const char *func, const char *msg, ...);

#define BM_MSG(cause, ...) bm_message(cause, __func__, __VA_ARGS__)

/** \brief Report a fatal error to the user and end the execution */
#define bm_fatal(...)                 \
    do                                \
    {                                 \
        BM_MSG("Fatal", __VA_ARGS__); \
        exit(1);                      \
    } while (0)

/** \brief Report an error to the user */
#define bm_error(...) BM_MSG("Error", __VA_ARGS__)

/** \brief Report a warning to the user */
#define bm_warn(...) BM_MSG("Warning", __VA_ARGS__)

/** \brief Report an info message to the user */
#define bm_info(...) BM_MSG("Info", __VA_ARGS__)

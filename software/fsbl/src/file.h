/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef FSBL_FILE_H
#define FSBL_FILE_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get file size in bytes.
 *
 * \param path File to examine
 *
 * \return File size on success, -1 otherwise.
 */
static inline long get_file_size(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp || fseek(fp, 0L, SEEK_END))
    {
        return -1;
    }

    long file_size = ftell(fp);
    fclose(fp);

    return file_size;
}

#ifdef __cplusplus
}
#endif

#endif // FSBL_FILE_H

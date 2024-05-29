/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "file.h"

#include <baremetal/common.h>
#include <baremetal/mem_barrier.h>
#include <baremetal/mp.h>
#include <baremetal/time.h>
#include <stdint.h>
#include <stdio.h>
#include <tiny_printf/printf.h>

// Step size 128 KiB chosen empirically - print output often enough, but
// don't slow down too much
#define STEP_SIZE 0x20000

/**
 * \brief Load contents of a file to memory
 *
 * \param fp Pointer to an open file to read
 * \param load_address Address to store the data at
 * \param file_size Size of the file, number of bytes to read
 *
 * \return Zero on success, nonzero otherwise
 */
static int do_load_file(FILE *fp, xlen_t load_address, size_t file_size)
{
    size_t        bytes_read;
    unsigned long bytes_total = 0;
    unsigned      time_start, time_end;

    time_start = bm_get_time_ms();

    do
    {
        bytes_read = fread((void *)load_address, 1, STEP_SIZE, fp);

        load_address += bytes_read;
        bytes_total += bytes_read;

        printf("\r> Loaded %3u%% of the file, %u bytes total",
               (unsigned)(bytes_total * 100 / file_size),
               (unsigned)bytes_total);

    } while (bytes_read == STEP_SIZE);

    time_end             = bm_get_time_ms();
    unsigned duration_ms = time_end - time_start;
    unsigned speed_kbps  = bytes_total / duration_ms;

    if (bytes_total != file_size)
    {
        printf("\rFailed to load the entire file, loaded %u from %u bytes.\n",
               (unsigned)bytes_total,
               (unsigned)file_size);
        return -1;
    }

    printf("\rFile loaded in %u ms, %u bytes total, speed %u KB/s\n",
           (unsigned)duration_ms,
           (unsigned)bytes_total,
           (unsigned)speed_kbps);
    return 0;
}

int load_sdcard_payload(xlen_t load_address, const char *path)
{
    long file_size = get_file_size(path);
    if (file_size < 0)
    {
        printf("Failed to get file size for '%s'\n", path);
        return -1;
    }

    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        printf("Failed to open file '%s'\n", path);
        return -1;
    }

    printf("Loading payload: addr=" BM_FMT_XLEN ", path=%s\n", load_address, path);
    int result = do_load_file(fp, load_address, file_size);

    bm_exec_fence();

    fclose(fp);
    return result;
}

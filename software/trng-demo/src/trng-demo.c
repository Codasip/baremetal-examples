/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "fatfs/ff.h"

#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <baremetal/trng.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef DATA_LEN
    #define DATA_LEN (100 * 1024 * 1024)
#endif

// FATFS fails with files larger 128mb
#define PART_SIZE (128 * 1024 * 1024)

// Buffer writes for efficiency
#define BUFF_SIZE (64 * 1024)

// Number of tested configurations
#define NUM_TESTS 2

// Tested configurations
static const bm_trng_config_t tests[NUM_TESTS] = {
    {// default settings
     .counter_sample_tero = 0x7f,
     .counter_clear_tero  = 0x84,
     .counter_wrap        = 0xff,
     .unbiasing           = true,
     .checking            = true },

    {// default settings without EC
     .counter_sample_tero = 0x7f,
     .counter_clear_tero  = 0x84,
     .counter_wrap        = 0xff,
     .unbiasing           = true,
     .checking            = false}
};

// Definition of file names used to store generated data
#define FILE_NAME "/rnd-dump-00-part-00.bin"
#define TEST_NUM1 10
#define TEST_NUM2 11
#define PART_NUM1 18
#define PART_NUM2 19

void fill_buffer(char *buff, unsigned len)
{
    static unsigned long data_generated = 0;
    static int           last_printout  = -1;

    bm_trng_t *tero = target_peripheral_get(BM_PERIPHERAL_TRNG);

    for (unsigned pos = 0; pos < len; pos += 4)
    {
        *(uint32_t *)(buff + pos) = bm_trng_get_rnd(tero);
    }

    data_generated += len;

    int percent = (unsigned long long)data_generated * 100 / (NUM_TESTS * DATA_LEN);

    if (percent > last_printout)
    {
        printf("\r%d%%", percent);
        fflush(stdout);

        last_printout = percent;
    }
}

int dump_part(const char *filename, unsigned long data_len)
{
    printf("\nDumping %lu bytes to file '%s'\n", data_len, filename);

    static FATFS fatfs;
    static FIL   fp;
    static char  buff[BUFF_SIZE];
    size_t       tmp;

    if (f_mount(&fatfs, "", 1) != FR_OK)
    {
        puts("Failed to mount file system");
        return -1;
    }

    if (f_open(&fp, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        puts("Failed to open file");
        f_unmount("");
        return -1;
    }

    while (data_len > 0)
    {
        unsigned buf_size = data_len > BUFF_SIZE ? BUFF_SIZE : data_len;

        fill_buffer(buff, buf_size);

        if (f_write(&fp, buff, buf_size, (UINT *)&tmp) != 0)
        {
            puts("Failed to write data");
            f_close(&fp);
            f_unmount("");
            return -1;
        }

        data_len -= buf_size;
    }

    f_close(&fp);
    f_unmount("");
    return 0;
}

int dump_data(unsigned long data_len)
{
    static char filename[] = FILE_NAME;

    filename[PART_NUM1] = '0';
    filename[PART_NUM2] = '0';

    while (data_len > 0)
    {
        unsigned long part_size = data_len > PART_SIZE ? PART_SIZE : data_len;
        if (dump_part(filename, part_size))
        {
            return -1;
        }
        data_len -= part_size;

        filename[PART_NUM2]++;
        if (filename[PART_NUM2] > '9')
        {
            filename[PART_NUM2] = '0';
            filename[PART_NUM1]++;
        }
    }

    filename[TEST_NUM2]++;
    if (filename[TEST_NUM2] > '9')
    {
        filename[TEST_NUM2] = '0';
        filename[TEST_NUM1]++;
    }

    return 0;
}

int main(void)
{
    puts("Welcome to the TRNG demo.");

    bm_counter_clear(BM_COUNTER_CYCLE);

    bm_trng_t *tero = target_peripheral_get(BM_PERIPHERAL_TRNG);

    for (unsigned i = 0; i < NUM_TESTS; i++)
    {
        printf("Generating data with configuration #%u", i);

        unsigned start_time = bm_get_time_ms();

        bm_trng_configure(tero, &tests[i]);

        if (dump_data(DATA_LEN))
        {
            return EXIT_FAILURE;
        }

        unsigned run_time = (bm_get_time_ms() - start_time) / 1000;
        printf("\nData generated in %um%us\n", run_time / 60, run_time % 60);
    }

    unsigned total_time = bm_get_time_ms() / 1000;
    printf("Finished, %uh%um%us elapsed\n", total_time / 3600, (total_time % 3600) / 60, total_time % 60);

    return EXIT_SUCCESS;
}

/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "fatfs/ff.h"

#include <baremetal/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Configuration
#define FILE_NAME            "stress_test_file.txt"
#define FILE_SIZE_MB         2 // Size of created file
#define BLOCK_SIZE           4096u
#define LOOP_COUNT           10 // Test loops count

#define TOTAL_BLOCKS         ((FILE_SIZE_MB * 1024 * 1024) / BLOCK_SIZE)

#define ENABLE_UNMOUNTS      1
#define ENABLE_VERIFICATIONS 1
#define ENABLE_FILE_CLEANUP  1

// Main Transfer buffer
static uint8_t buffer[BLOCK_SIZE];

const char *fatfs_strerror(FRESULT result)
{
    switch (result)
    {
        case FR_OK:
            return "Succeeded";
        case FR_DISK_ERR:
            return "A hard error occurred in the low level disk I/O layer";
        case FR_INT_ERR:
            return "Assertion failed";
        case FR_NOT_READY:
            return "The physical drive cannot work";
        case FR_NO_FILE:
            return "Could not find the file";
        case FR_NO_PATH:
            return "Could not find the path";
        case FR_INVALID_NAME:
            return "The path name format is invalid";
        case FR_DENIED:
            return "Access denied or directory full";
        case FR_EXIST:
            return "File/directory already exists";
        case FR_INVALID_OBJECT:
            return "The file/directory object is invalid";
        case FR_WRITE_PROTECTED:
            return "The physical drive is write protected";
        case FR_INVALID_DRIVE:
            return "The logical drive number is invalid";
        case FR_NOT_ENABLED:
            return "The volume has no work area";
        case FR_NO_FILESYSTEM:
            return "There is no valid FAT volume";
        case FR_MKFS_ABORTED:
            return "f_mkfs() aborted due to a problem";
        case FR_TIMEOUT:
            return "Timeout: couldn't access volume in time";
        case FR_LOCKED:
            return "Operation rejected due to file sharing policy";
        case FR_NOT_ENOUGH_CORE:
            return "LFN working buffer could not be allocated";
        case FR_TOO_MANY_OPEN_FILES:
            return "Too many open files (FF_FS_LOCK exceeded)";
        case FR_INVALID_PARAMETER:
            return "Invalid parameter";
        default:
            return "Unknown FATFS result";
    }
}

void fill_pattern(uint8_t *buf, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        buf[i] = (uint8_t)(i % 0xff);
    }
}

bool verify_pattern(const uint8_t *buf, size_t size, size_t *mismatch_offset)
{
    for (size_t i = 0; i < size; i++)
    {
        if (buf[i] != (uint8_t)(i % 0xff))
        {
            *mismatch_offset = i; // Update mismatch offset value when error occured
            return false;
        }
    }

    return true;
}

bool file_cleanup(void)
{
    printf("File cleanup...\n");

    // Remove file if it exists
    FRESULT res = f_stat(FILE_NAME, NULL);

    if (res == FR_OK)
    {
        printf("File found. Deleting it...\n");
        res = f_unlink(FILE_NAME);
        if (res != FR_OK)
        {
            printf("Failed to delete existing file\n");
            printf(" - FRESULT:    0x%02x (%s)\n", res, fatfs_strerror(res));
            return false;
        }
    }

    return true;
}

bool mount_card(FATFS *fs)
{
    printf("Mounting the SD card...\n");

    FRESULT res = f_mount(fs, "", 1); // Try to mount the SD card

    if (res != FR_OK)
    {
        printf("Failed to mount filesystem\n");
        printf(" - FRESULT:    0x%02x (%s)\n", res, fatfs_strerror(res));
        return false;
    }

    return true;
}

bool unmount_card(void)
{
    printf("Unmounting the SD card...\n");
    FRESULT res = f_mount(NULL, "", 0); // unmount

    if (res != FR_OK)
    {
        printf("Failed to unmount filesystem\n");
        printf(" - FRESULT:    0x%02x (%s)\n", res, fatfs_strerror(res));
        return false;
    }

    return true;
}

bool write_test_phase(void)
{
    FIL     f;
    FRESULT res;

    printf("Opening file...\n");
    res = f_open(&f, FILE_NAME, FA_WRITE | FA_CREATE_ALWAYS); // Try to create/open the file
    if (res != FR_OK)
    {
        printf("Failed to open file for writing\n");
        printf(" - FRESULT:    0x%02x (%s)\n", res, fatfs_strerror(res));
        return false;
    }

    printf("Writing to the file...\n");

    uint32_t start_ms = bm_get_time_ms(); // Get start time in ms

    for (size_t i = 0; i < (size_t)TOTAL_BLOCKS; i++)
    {
        fill_pattern(buffer, BLOCK_SIZE);

        size_t total_written = 0;
        while (total_written < BLOCK_SIZE)
        {
            UINT chunk = 0;
            res        = f_write(&f, &buffer[total_written], BLOCK_SIZE - total_written, &chunk);
            if (res != FR_OK)
            {
                printf("Write error at block %zu\n", i);
                printf(" - FRESULT:    0x%02x (%s)\n", res, fatfs_strerror(res));
                printf("Closing file...\n");
                f_close(&f);
                return false;
            }

            if (chunk == 0)
            {
                printf("Zero-byte write at block %zu (written %zu of %u)\n", i, total_written, BLOCK_SIZE);
                f_close(&f);
                return false;
            }

            total_written += chunk;
        }
    }

    unsigned end_ms = bm_get_time_ms(); // Get end time in ms

    printf("Closing file...\n");
    f_close(&f); // Close the file

    unsigned write_time = end_ms - start_ms; // Calculate write time
    unsigned kb_written = FILE_SIZE_MB * 1024;
    unsigned kbps       = (write_time > 0) ? (kb_written * 1000) / write_time : 0;
    printf("Write complete in %u ms (%u KB/s)\n", write_time, kbps);

    return true;
}

bool read_verify_test_phase(void)
{
    FIL     f;
    FRESULT res;

    printf("Opening file...\n");
    res = f_open(&f, FILE_NAME, FA_READ); // Try to open the file
    if (res != FR_OK)
    {
        printf("Failed to open file for reading\n");
        printf(" - FRESULT:    0x%02x (%s)\n", res, fatfs_strerror(res));
        return false;
    }

    printf("Reading from the file...\n");

    unsigned start_ms = bm_get_time_ms(); // Get start time in ms

    for (size_t i = 0; i < (size_t)TOTAL_BLOCKS; i++)
    {
        size_t total_read = 0;

        while (total_read < BLOCK_SIZE)
        {
            UINT chunk = 0;
            res        = f_read(&f, &buffer[total_read], BLOCK_SIZE - total_read, &chunk);
            if (res != FR_OK)
            {
                printf("Read error at block %zu\n", i);
                printf(" - FRESULT:    0x%02x (%s)\n", res, fatfs_strerror(res));
                return false;
            }

            if (chunk == 0)
            {
                printf("Unexpected EOF at block %zu (read %zu of %u)\n", i, total_read, BLOCK_SIZE);
                return false;
            }

            total_read += chunk;
        }

#if (ENABLE_VERIFICATIONS == 1)
        // Verify pattern
        size_t offset = 0;
        if (!verify_pattern(buffer, BLOCK_SIZE, &offset))
        {
            printf("Verification failed at block %zu, offset %zu\n", i, offset);
            return false;
        }
#endif
    }

    unsigned end_ms = bm_get_time_ms(); // Get end time in ms

    printf("Closing file...\n");
    f_close(&f);

    unsigned read_time  = end_ms - start_ms; // Calculate read time
    unsigned kb_written = FILE_SIZE_MB * 1024;
    unsigned kbps       = (read_time > 0) ? (kb_written * 1000) / read_time : 0;
    printf("Write complete in %u ms (%u KB/s)\n", read_time, kbps);

    return true;
}

bool run_stress_test(void)
{
    FATFS fs;

#if (ENABLE_UNMOUNTS == 0)
    if (!mount_card(&fs))
    {
        return false;
    }
#endif

    // Start stress test loop
    for (int loop = 0; loop < LOOP_COUNT; loop++)
    {
        printf("Loop %d...\n", loop);

#if (ENABLE_UNMOUNTS == 1)
        if (!mount_card(&fs))
        {
            return false;
        }
#endif

#if (ENABLE_FILE_CLEANUP == 1)
        if (!file_cleanup())
        {
            return false;
        }
#endif

        if (!write_test_phase())
        {
            return false;
        }

        if (!read_verify_test_phase())
        {
            return false;
        }

#if (ENABLE_FILE_CLEANUP == 1)
        if (!file_cleanup())
        {
            return false;
        }
#endif

#if (ENABLE_UNMOUNTS == 1)
        if (!unmount_card())
        {
            return false;
        }
#endif

        printf("Loop %d passed \n\n", loop);
    }

    return true;
}

int main(void)
{
    puts("Welcome to the SD-card demo.\n");

    printf("Starting SD card stress test demo...\n");

    // Run SD card stress test
    if (!run_stress_test())
    {
        printf("SD card test failed");
        return EXIT_FAILURE;
    }

    printf("SD card test passed successfully\n");

    return EXIT_SUCCESS;
}

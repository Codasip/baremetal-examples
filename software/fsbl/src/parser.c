/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "parser.h"

#include <baremetal/common.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tiny_printf/printf.h>

#define ENTRY_BUFFER_SIZE 1024

#define SKIP_WHITE(str)                    \
    while (isspace(*(unsigned char *)str)) \
    {                                      \
        str++;                             \
    }
#define SKIP_NON_WHITE(str)                                               \
    while (!isspace(*(unsigned char *)str) && *(unsigned char *)str != 0) \
    {                                                                     \
        str++;                                                            \
    }

/* Internal buffer for storing config file entries */
static char entry_buffer[ENTRY_BUFFER_SIZE];

/* Internal parser state */
static FILE    *fp;
static unsigned line;

/**
 * \brief Print info about a configuration entry
 *
 * \param line Line number in the config file
 * \param entry Pointer to the entry
 * \param msg Additional message to print out
 * \param offset Position in the line to highlight
 */
static void print_entry_info(unsigned line, const char *entry, const char *msg, int offset)
{
    printf("Line %u:\n%s\n", line, entry);
    if (offset >= 0)
    {
        printf("%*c^ ", offset, ' ');
    }
    printf("%s\n\n", msg);
}

int init_parser(const char *path)
{
    line = 0;
    fp   = fopen(path, "r");

    if (!fp)
    {
        return -1;
    }

    return 0;
}

void finish_parser(void)
{
    fclose(fp);
}

int parse_entry(entry_t *entry)
{
    while (fgets(entry_buffer, ENTRY_BUFFER_SIZE, fp))
    {
        line++;
        char *end, *pos = entry_buffer;

        // Skip comments and blank lines
        SKIP_WHITE(pos)
        if (*pos == 0 || *pos == '#')
        {
            continue;
        }

        // Parse switch number
        int sw      = strtol(pos, &end, 10);
        entry->gpio = (end != pos) ? sw : -1;
        pos         = end;

        // Parse payload path
        SKIP_WHITE(pos)
        entry->path = (char *)pos;
        SKIP_NON_WHITE(pos)
        if (*pos == 0)
        {
            printf("\nInvalid FSBL configuration entry.\n");
            print_entry_info(line,
                             entry_buffer,
                             "Payload missing load address.",
                             (xlen_t)pos - (xlen_t)entry_buffer);
            continue;
        }
        char *path_end = pos;
        pos++;

        // Parse load address
        SKIP_WHITE(pos)
        unsigned long num = strtoul(pos, &end, 16);
        if (end == pos)
        {
            printf("\nInvalid FSBL configuration entry.\n");
            print_entry_info(line,
                             entry_buffer,
                             "Payload missing load address.",
                             (xlen_t)pos - (xlen_t)entry_buffer);
            continue;
        }
        entry->load_addr = num;
        pos              = end;

        // Parse BOOT, FDT or NXT
        SKIP_WHITE(pos)
        entry->flags = 0;
        if (!strncmp(pos, "BOOT", 4))
        {
            entry->flags |= ENTRY_FLAG_BOOT;
            pos += 4;
        }
#ifdef TARGET_LINUX_SUPPORT
        else if (!strncmp(pos, "FDT", 3))
        {
            entry->flags |= ENTRY_FLAG_FDT;
            pos += 3;
        }
        else if (!strncmp(pos, "NXT", 3))
        {
            entry->flags |= ENTRY_FLAG_NXT;
            pos += 3;
        }
#endif

        // Check for EOF / EOL
        SKIP_WHITE(pos)
        if (*pos != 0)
        {
            printf("\nDubious FSBL configuration entry.\n");
            print_entry_info(line,
                             entry_buffer,
                             "Unmatched characters ignored.",
                             (xlen_t)pos - (xlen_t)entry_buffer);
        }

        *path_end = 0;
        return line;
    }
    return 0;
}

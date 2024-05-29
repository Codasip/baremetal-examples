/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef FSBL_PARSER_H
#define FSBL_PARSER_H

#define ENTRY_FLAG_BOOT 0x1
#define ENTRY_FLAG_FDT  0x2
#define ENTRY_FLAG_NXT  0x4

typedef struct {
    int           gpio;
    char         *path;
    unsigned long load_addr;
    unsigned      flags;
} entry_t;

/**
 * \brief Start parsing a file
 *
 * \param path File to parse
 *
 * \return Zero on success, nonzero otherwise
 */
int init_parser(const char *path);

/**
 * \brief Parse a single entry from a file
 *
 * \param entry Entry structure to store the parsed results in
 *
 * \return Line number of the entry (indexed from one) on success, zero otherwise
 */
int parse_entry(entry_t *entry);

/**
 * \brief Finalize parsing of a file
 */
void finish_parser(void);

#endif // FSBL_PARSER_H

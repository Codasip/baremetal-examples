/* Copyright 2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef SYS_HW_H
#define SYS_HW_H

/**
 * \brief A simple CLI gets() style function using UART peripheral
 *
 * \param s Pointer to the output string buffer
 * \param n size of the buffer s
 *
 * \return Pointer to the string s, NULL on error
 */
char *cli_gets(char *s, uint32_t n);

#endif /* !SYS_HW_H */

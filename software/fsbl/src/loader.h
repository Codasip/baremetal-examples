/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef FSBL_LOADER_H
#define FSBL_LOADER_H

#include <baremetal/common.h>

/**
 * \brief Load payload from SD card to memory
 *
 * \param load_address Address to store the payload at
 * \param path File to load from SD card
 *
 * \return Zero on success, nonzero otherwise
 */
int load_sdcard_payload(xlen_t load_address, const char *path);

#endif // FSBL_LOADER_H

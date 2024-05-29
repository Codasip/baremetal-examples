/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef MMC_H
#define MMC_H

#include "diskio.h"

void mmc_test(void);

DSTATUS MMC_disk_initialize(void);
DSTATUS MMC_disk_status(void);
DRESULT MMC_disk_read(BYTE *buff, LBA_t sector, UINT count);

#endif /* MMC_H */

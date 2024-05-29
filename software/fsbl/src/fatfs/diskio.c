/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "ff.h"
#include "diskio.h"

#include <stdint.h>
#include <string.h>

#include "mmc.h"

#define DISK_MMC	0

DSTATUS disk_initialize(BYTE pdrv)
{
	switch (pdrv) {
	case DISK_MMC:
		return MMC_disk_initialize();
	default:
		return STA_NODISK;
	}
}

DSTATUS disk_status (BYTE pdrv)
{
	switch (pdrv) {
	case DISK_MMC:
		return MMC_disk_status();
	default:
		return STA_NOINIT;
	}

	return 0;
}

DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	switch (pdrv) {
	case DISK_MMC:
		return MMC_disk_read(buff, sector, count);
	default:
		return RES_PARERR;
	}
}

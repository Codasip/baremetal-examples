/**
 * Codasip s.r.o.
 *
 * CONFIDENTIAL
 *
 * Copyright 2024 Codasip s.r.o.
 *
 * All Rights Reserved.
 * This file is part of a Codasip product. No part of this file may be use, copied,
 * modified, or distributed except in accordance with the terms contained in the
 * Codasip license agreement under which you obtained this file.
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <baremetal/common.h>
#include <stdint.h>

/** Specify loop unrolling parameter */
#define UNROLL_LEN 256

/** How many accesses to perform */
#define NUM_ITERATIONS 100000

/** Start-end memory address to use for performance-test access */
#define DDR_ADDR_START  0x80000000
#define DDR_ADDR_END    0xC0000000

#define SRAM_ADDR_START 0x20000000
#define SRAM_ADDR_END   0x20100000

#endif /* CONFIG_H_ */

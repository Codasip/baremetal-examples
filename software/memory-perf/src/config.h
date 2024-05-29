/**
 * Codasip s.r.o.
 *
 * CONFIDENTIAL
 *
 * Copyright 2023 Codasip s.r.o.
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

/** Type specifying memory access width */
#define width_t xlen_t

/** Specify loop unrolling parameter */
#define UNROLL_LEN 256

/** How many accesses to perform */
#define NUM_ITERATIONS 1000000

/** Start memory address to use for performance-test access */
#define ADDR_START 0x80000000

#endif /* CONFIG_H_ */

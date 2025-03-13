/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * 
 * Changes: Copyright (C) 2024 Codasip s.r.o.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://www.github.com/FreeRTOS
 */

#ifndef RISCV_CODASIP_H_
#define RISCV_CODASIP_H_

#include "core_info.h"
#include "platform_info.h"
#include "unified_int_mem_map.h"

#include "riscv-reg.h"

#ifdef __ASSEMBLER__
    #define CONS(NUM, TYPE) NUM
#else
    #define CONS2(NUM, TYPE) NUM##TYPE
    #define CONS(NUM, TYPE)  CONS2(NUM, TYPE)
#endif /* __ASSEMBLER__ */

#define PRIM_HART 0

/* Codasip Unified Memory Map */
// TARGET_TIMER_ADDR switches between BM_CLINT_ADDR and BM_ACLINT_ADDR depending on the Platform
#define CONF_CLINT_ADDR     CONS(TARGET_TIMER_ADDR, UL)
#define CONF_CLINT_MSIP     CONS(0x0000, UL)
#define CONF_CLINT_MTIMECMP CONS(0x4000, UL)
#define CONF_CLINT_MTIME    CONS(0xbff8, UL)

#ifndef __ASSEMBLER__

int  xGetCoreID(void);
void vSendString(const char *s);

#endif /* __ASSEMBLER__ */

#endif /* RISCV_CODASIP_H_ */

/*
 * Copyright (C) 2024 Codasip s.r.o.
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
 */

#ifndef __CMSIS_FREERTOS_DEMO_H__
#define __CMSIS_FREERTOS_DEMO_H__

/* Application Configuration */

/* UART Selection */
#define UART_LOCAL_USE 1 /* 1 = UART setup in this file; 0 = Use SYS UART (via printf) */

/* Local UART IRQ enable */
#define UART_LOCAL_IRQ_ENABLE 0 /* If UART_LOCAL_USE == 1, then use IRQ when = 1 */

#ifndef TARGET_HAS_CLIC
    /* IRQ Setup: Vector Table or Direct (Not used for CLIC) */
    #define mainVECTOR_MODE_DIRECT 0 /* 0 = IRQ Vector Table, 1 = Direct setup */
#endif

/* Exception Handler Selection */
#define EXCEPTION_HANDLER_LOCAL_USE 0 /* 0 = Handle locally in this file, 1 = use BM Handler */

#endif /* !__CMSIS_FREERTOS_DEMO_H__ */

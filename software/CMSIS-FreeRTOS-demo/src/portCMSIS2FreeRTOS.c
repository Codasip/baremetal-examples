/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include "CMSIS-FreeRTOS-demo.h"

/* FreeRTOS kernel includes */
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <semphr.h>
#include <task.h>

/* Codasip BareMetal includes */
#include <baremetal/interrupt.h>
extern void bm_managed_handler_inner(bm_priv_mode_t new_mode);

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
    /* vApplicationMallocFailedHook() will only be called if
     * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
     * function that will get called if a call to pvPortMalloc() fails.
     * pvPortMalloc() is called internally by the kernel whenever a task, queue,
     * timer or semaphore is created.  It is also called by various parts of the
     * demo application.  If heap_1.c or heap_2.c are used, then the size of the
     * heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
     * FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
     * to query the size of free heap space that remains (although it does not
     * provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();

    for (;;)
    {}
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
     * to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
     * task.  It is essential that code added to this hook function never attempts
     * to block in any way (for example, call xQueueReceive() with a block time
     * specified, or call vTaskDelay()).  If the application makes use of the
     * vTaskDelete() API function (as this demo application does) then it is also
     * important that vApplicationIdleHook() is permitted to return to its calling
     * function, because it is the responsibility of the idle task to clean up
     * memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)pxTask;

    /* Run time stack overflow checking is performed if
     * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
     * function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();

    for (;;)
    {}
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void) {}
/*-----------------------------------------------------------*/

void vAssertCalled(void)
{
    volatile uint32_t ulSetTo1ToExitFunction = 0;

    taskDISABLE_INTERRUPTS();

    while (ulSetTo1ToExitFunction != 1)
    {
        __asm volatile("NOP");
    }
}
/*-----------------------------------------------------------*/

PORT_NOTIFY_ISR_DEFINE;

void freertos_risc_v_application_interrupt_handler(uint32_t ulMcause)
{
    (void)ulMcause;

    PortNotifyISRBegin();

    /* Call the BareMetal configured internal Interrupt Handler 
     * via the bm_interrupt_handler_table[offset]().
     * 
     * The "Internal" Interrupt BM_INTERRUPT_MEIP was set [via bm_interrupt_set_handler() in main()]
     * to call the external Interrupt Handler via bm_ext_irq_handler(). */
    bm_managed_handler_inner(BM_PRIV_MODE_MACHINE);

    PortNotifyISREnd();
}
/*-----------------------------------------------------------*/

void freertos_risc_v_application_exception_handler(uint32_t ulMcause)
{
    (void)ulMcause;

#if EXCEPTION_HANDLER_LOCAL_USE
    char pcCause[20];

    /* Not implemented yet! */

    sprintf(pcCause, "%u", ulMcause);

    #if UART_LOCAL_USE
    /* This is not protected by a mutex */
    write_line(pcCause);

    #else
    /* This is not protected by a mutex */
    printf("%s\r\n", pcCause);
    #endif

    configASSERT(ulMcause == 0);

#else  /* !EXCEPTION_HANDLER_LOCAL_USE */
    /* Call the BareMetal configured Exception Handler 
     * via bm_exc_handler_table[offset]() or error. */
    bm_managed_handler_inner(BM_PRIV_MODE_MACHINE);
#endif /* !EXCEPTION_HANDLER_LOCAL_USE */
}

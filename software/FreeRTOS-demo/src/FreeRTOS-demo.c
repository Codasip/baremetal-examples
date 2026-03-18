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

/* FreeRTOS kernel includes */
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <semphr.h>
#include <task.h>

/* Codasip BareMetal includes */
#include <baremetal/bm_cheri.h>
#include <baremetal/gpio.h>
#include <baremetal/interrupt.h>
#include <baremetal/mp.h>
#include <baremetal/platform.h>
#include <baremetal/uart.h>
#include <tiny_printf/printf.h>

#ifdef __CHERI_PURE_CAPABILITY__
    #include <cheriintrin.h>
#endif

/* UART Selection */
#define UART_LOCAL_USE 0 /* 1 = UART setup in this file; 0 = Use SYS UART (via printf) */

/* Local UART IRQ enable */
#define UART_LOCAL_IRQ_ENABLE 1 /* If UART_LOCAL_USE == 1, then use IRQ when = 1 */

#ifndef TARGET_HAS_CLIC
    /* IRQ Setup: Vector Table or Direct (Not used for CLIC) */
    #define mainVECTOR_MODE_DIRECT 0 /* 0 = IRQ Vector Table, 1 = Direct setup */
#endif

/* Exception Handler Selection */
#define EXCEPTION_HANDLER_LOCAL_USE 0 /* 0 = Handle locally in this file, 1 = use BM Handler */

/*-----------------------------------------------------------*/

extern void freertos_risc_v_trap_handler(void);
extern void freertos_vector_table(void);
extern int  main_blinky(void);
extern int  main_buggy(void);

extern void bm_ext_irq_handler(bm_register_file_t *stacked_regs);
extern void bm_managed_handler_inner(bm_priv_mode_t new_mode, bm_register_file_t *stacked_regs);

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

/*-----------------------------------------------------------*/

#ifdef TARGET_HAS_CLIC
    #include <baremetal/clic.h>
    #include <baremetal/mem_barrier.h>

extern void freertos_risc_v_mtimer_interrupt_handler(void);

    #ifdef __CHERI_PURE_CAPABILITY__
/* For Cheri the vector table entries are not capabilities, they are just XLEN integer addresses.
 * The capability is created by the Cheri CLIC when an interrupt is taken.
 * From the documentation:
 *   When vectoring interrupts through the Trap Vector Table, the entry selected will be at location
 *   TBASE + (XLEN/8)*exccode, where TBASE is the address in Xtvtc and exccode is the interrupt id
 *   that will be reported in Xcause.
 *   Once the entry has been fetched, the ISR’s entry point capability is installed into PCC.
 *   This is formed by taking the address tvtentry.ENTRYPOINT and applying scaddr (Capability Set
 *   Address) instruction semantics to the capability held in the Xtvtentryic CSR selected
 *   by tvtentry.SELECTC.
 *
 * You can further bound all Interrupt Service Routine (ISR) entry points present in
 * the Trap Vector Table by writing a capability to CSR Xtvtentryic, e.g. mtvtentry0c for machine
 * mode. But you will need to write an infinite cap to Xtvtentryic if you want to
 * run your ISR in Cheri cap mode (as it defaults to the integer infinite cap).
 */
static xlen_t mtvt_table[TARGET_CLIC_NUM_INPUTS] __attribute__((aligned(64))) = {0};

    #else
static void (*mtvt_table[TARGET_CLIC_NUM_INPUTS])(void) __attribute__((aligned(64))) = {0};
    #endif

static bm_clic_t *clic;
#endif

/*-----------------------------------------------------------*/

int xGetCoreID(void)
{
    int id;

    // __asm ("csrr %0, mhartid" : "=r" ( id ) );
    id = bm_get_hartid(); /* Use the BareMetal version */

    return id;
}

/*-----------------------------------------------------------*/

#if UART_LOCAL_USE
static bm_uart_t *uart;

/**
 * \brief Helper function for printing out a whole string over UART
 *
 * \param str String to print
 */
static inline void write_line(const char *str)
{
    while (*str != 0)
    {
        bm_uart_transmit_byte(uart, *str++);
    }
}
#endif

/*-----------------------------------------------------------*/

/**
 * \brief Interrupt handler for UART
 */
#if UART_LOCAL_USE && UART_LOCAL_IRQ_ENABLE
void uart_interrupt_handler(void)
{
    bm_uart_handle_irq(uart);
}
#endif

/*-----------------------------------------------------------*/

#if UART_LOCAL_USE
static void vUartInit(void)
{
    uart = (bm_uart_t *)target_peripheral_get(BM_PERIPHERAL_UART_CONSOLE);

    bm_uart_config_t config = {.baud_rate   = 115200,
                               .data_format = BM_UART_DATA_BITS_8,
                               .parity      = BM_UART_PARITY_NONE,
                               .stop        = BM_UART_STOP_BITS_1,
                               .use_irq     = UART_LOCAL_IRQ_ENABLE};
    bm_uart_init(uart, &config);

    #if UART_LOCAL_IRQ_ENABLE

    bm_ext_irq_set_handler(uart->ext_irq_id, uart_interrupt_handler);

        #ifndef TARGET_HAS_CLIC

    // PIC/PLIC is connected to core's external interrupt, enable it.
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MEIP);

        #endif // !TARGET_HAS_CLIC

    #endif
}
#endif

/*-----------------------------------------------------------*/

static SemaphoreHandle_t xSemaphoreSendString;

/* DO NOT CALL FROM AN ISR as this is using a FreeRTOS mutex */
void vSendString(const char *s)
{
    xSemaphoreTake(xSemaphoreSendString, portMAX_DELAY);

#if UART_LOCAL_USE
    /* Use write_line() as baremetal-examples's printf(), which calls _write(), self initializes
     * the UART (syscalls/sys_uart.c) */
    write_line(s);
#else
    /* BareMetal SYS UART _write() (used by printf()) self initializes, so you can just use printf
     * after setting up bm_interrupt_tvec_setup() */
    printf("%s", s);
#endif

    xSemaphoreGive(xSemaphoreSendString);
}

#if defined(DEMO_BLINKY)
    /*-----------------------------------------------------------*/

    /* Priority used by the task */
    #define mainFLASH_LEDS_TASK_PRIORITY (tskIDLE_PRIORITY + 2)

    /* The rate at which to flash the LEDs.  The 250ms value is converted
     * to ticks using the pdMS_TO_TICKS() macro. */
    #define mainFLASH_LEDS_FREQUENCY_MS pdMS_TO_TICKS(250)

static void prvFlashLEDsTask(void *pvParameters)
{
    /* Remove compiler warning about unused parameter. */
    (void)pvParameters;

    TickType_t xNextWakeTime;
    int        f = 1;
    /* Initialize xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    vSendString("This (Fx) task is the BareMetal GPIO demo in a FreeRTOS task\n");
    vSendString("with an additional counting number output.\n");
    vSendString("Feel free to flip switches 1-3, and observe blinking LEDs.\n\n");

    vSendString("The other (Tx/Rx) tasks are the standard FreeRTOS Demo app (main_blinky.c)\n\n");

    bm_gpio_t *gpio = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);

    bool light = false;

    for (;;)
    {
        char buf[40];

        sprintf(buf, "%d: %s: %d\n", xGetCoreID(), pcTaskGetName(xTaskGetCurrentTaskHandle()), f);
        vSendString(buf);

        f++;

        light = !light;

        // Blink LED 0 and also LEDs 1-3 if the corresponding switches are on
        if (light)
        {
            bm_gpio_set(gpio, TARGET_GPIO_PORT_LED0);

            if (bm_gpio_read(gpio, TARGET_GPIO_PORT_SWITCH1))
            {
                bm_gpio_set(gpio, TARGET_GPIO_PORT_LED1);
            }
            if (bm_gpio_read(gpio, TARGET_GPIO_PORT_SWITCH2))
            {
                bm_gpio_set(gpio, TARGET_GPIO_PORT_LED2);
            }
            if (bm_gpio_read(gpio, TARGET_GPIO_PORT_SWITCH3))
            {
                bm_gpio_set(gpio, TARGET_GPIO_PORT_LED3);
            }
        }
        else
        {
            bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED0);
            bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED1);
            bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED2);
            bm_gpio_clear(gpio, TARGET_GPIO_PORT_LED3);
        }

        /* Place this task in the blocked state until it is time to run again. */
        vTaskDelayUntil(&xNextWakeTime, mainFLASH_LEDS_FREQUENCY_MS);
    }
}

/*-----------------------------------------------------------*/
#endif

int main(void)
{
    int ret;

#ifdef __CHERI_PURE_CAPABILITY__
    vPortInitialiseCheri(cheri_ddc_get());
#endif

    /* Do not call: bm_interrupt_init() as we are using FreeRTOS interrupt handling first */
    // bm_interrupt_init(BM_PRIV_MODE_MACHINE);

    /* Do not call: bm_interrupt_set_handler() as FreeRTOS enables MEIP Interrupts when
     * the scheduler starts.
     *
     * Also, do not printf() anything before vTaskStartScheduler() because we are using
     * the FreeRTOS interrupt handling first and it is not setup until vTaskStartScheduler().
     */

#ifdef TARGET_HAS_CLIC
    /* CLIC trap (interrupt and exception) handler initialization */

    /* With the Codasip CLIC all interrupts and exceptions will call freertos_risc_v_trap_handler()
     * unless an interrupt is set to vectored mode, upon which it's up to the developer
     * to wrap their ISR correctly if it interacts with FreeRTOS */

    bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE,
                            (xlen_t)freertos_risc_v_trap_handler,
                            BM_INTERRUPT_MODE_DIRECT); /* BM_INTERRUPT_MODE_DIRECT has no effect
                                                           with the Codasip CLIC as the lower 6 bits
                                                           are hardwired to xtvec[5:0] = 000011b
                                                           (CLIC Mode) */

    /** Setup the interrupt vector table */
    clic = (bm_clic_t *)target_peripheral_get(BM_PERIPHERAL_CLIC);
    bm_clic_init(clic);

    /* mtvt_table[0] = your_clint_sw_interrupt_handler; */
    #ifdef __CHERI_PURE_CAPABILITY__
    mtvt_table[1] = (xlen_t)freertos_risc_v_mtimer_interrupt_handler; /* CLINT mtimer (IRQ 1) vector entry */

    #else
    mtvt_table[1] = freertos_risc_v_mtimer_interrupt_handler; /* CLINT mtimer (IRQ 1) vector entry */
    #endif

    /* Setup any remaining interrupt vectors here, e.g.:
     * mtvt_table[2 upwards] = your_isr_handler_X; */

    #ifdef __CHERI_PURE_CAPABILITY__
    BM_CSR_WRITE_CAP2(0x307, addr_to_code_ptr((uintptr_t)mtvt_table));

    __asm__ volatile( // "csrrc	ca0, ddc, zero\n" // Read  infinite cap from ddc (here for reference)
        "auipc  ca0, 0x0\n"       // Read  infinite cap from PC
        "csrrw   x0, 0x7f8, a0\n" // Write infinite cap to mtvtentry0c
        "csrrw   x0, 0x7f9, x0\n" // Write null     cap to mtvtentry1c
        ::
            : "ca0");

    #else
    xlen_t val    = (xlen_t)mtvt_table;
    BM_CSR_WRITE(BM_CSR_MTVT, val);
    #endif

    bm_exec_fence_i();

    /* Set any interrupts to vectored mode, if required */
    bm_clic_set_vectored(clic, 1, 1); /* For CLINT mtimer (IRQ 1) to call freertos_risc_v_mtimer_interrupt_handler() */

    /* Enable external interrupts, if required */
    bm_clic_set_enable(clic, 1, 1); /* For CLINT mtimer (IRQ 1) to call freertos_risc_v_mtimer_interrupt_handler() */

#else /* !TARGET_HAS_CLIC */
    /* Non-CLIC trap (interrupt and exception) handling */

    /* trap handler initialization */
    #if (mainVECTOR_MODE_DIRECT == 1)
    {
        bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE,
                                (xlen_t)freertos_risc_v_trap_handler,
                                BM_INTERRUPT_MODE_DIRECT);
    }
    #else
    {
        bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE,
                                (xlen_t)freertos_vector_table,
                                BM_INTERRUPT_MODE_VECTOR);
    }
    #endif

    bm_interrupt_set_handler(BM_INTERRUPT_MEIP, bm_ext_irq_handler);
#endif /* !TARGET_HAS_CLIC */

#if UART_LOCAL_USE
    vUartInit();
#endif

    /* Create a mutex type semaphore. */
    xSemaphoreSendString = xSemaphoreCreateMutex();
    vSendString("======================================================\n");
    vSendString("FreeRTOS Version " tskKERNEL_VERSION_NUMBER "\n");
    vSendString("BareMetal Examples Version " BUILD_VERSION "\n\n");
#if defined(DEMO_BLINKY)
    vSendString("FreeRTOS multi-task example running\n");
    vSendString("3 tasks outputting text with 1 also flashing LEDs.\n");
    vSendString("======================================================\n\n");
    xTaskCreate(prvFlashLEDsTask,
                "Fx",
                configMINIMAL_STACK_SIZE * 2U,
                NULL,
                mainFLASH_LEDS_TASK_PRIORITY,
                NULL);
    ret = main_blinky();
#elif defined(DEMO_BUGGY)
    vSendString("FreeRTOS buggy buffers overflow example running 3\n");
    vSendString("tasks overflowing buffers, to demonstrate the ability\n");
    vSendString("to catch faults and recover safely when using CHERI.\n\n");
    #ifdef __CHERI_PURE_CAPABILITY__
    vSendString("Running in CHERI Pure-Capability mode.\n\n");
    vSendString("Detection of buffer overflows is possible, and\n");
    vSendString("sensitive data is protected!\n");
    #else
    vSendString("Running in non-CHERI mode.\n\n");
    vSendString("Detection of buffer overflows is not possible, and\n");
    vSendString("sensitive data may be corrupted!\n");
    #endif
    vSendString("======================================================\n\n");
    ret = main_buggy();
#else
    #error "Please add or select demo."
#endif

    return ret;
}

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

void freertos_risc_v_application_interrupt_handler(uint32_t ulMcause)
{
    (void)ulMcause;

    /* Call the BareMetal configured internal Interrupt Handler
     * via the bm_interrupt_handler_table[offset]().
     *
     * The "Internal" Interrupt BM_INTERRUPT_MEIP was set [via bm_interrupt_set_handler() in main()]
     * to call the external Interrupt Handler via bm_ext_irq_handler(). */
    bm_register_file_t stacked_regs_dummy = {0};
    bm_managed_handler_inner(BM_PRIV_MODE_MACHINE, &stacked_regs_dummy);
}
/*-----------------------------------------------------------*/

void __attribute__((weak)) freertos_risc_v_application_exception_handler(uint32_t ulMcause)
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
    bm_register_file_t stacked_regs_dummy = {0};
    bm_managed_handler_inner(BM_PRIV_MODE_MACHINE, &stacked_regs_dummy);
#endif /* !EXCEPTION_HANDLER_LOCAL_USE */
}

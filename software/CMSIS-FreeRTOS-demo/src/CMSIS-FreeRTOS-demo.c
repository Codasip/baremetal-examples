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

#include <string.h>

/* FreeRTOS kernel includes */
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <semphr.h>
#include <task.h>

/* CMSIS-RTOS2 Wrapper API */
#include "cmsis_os2.h"

#include "CMSIS-FreeRTOS-demo.h"

/* Codasip BareMetal includes */
#include <baremetal/gpio.h>
#include <baremetal/interrupt.h>
#include <baremetal/mp.h>
#include <baremetal/platform.h>
#include <baremetal/uart.h>
#include <tiny_printf/printf.h>

/*-----------------------------------------------------------*/

extern void freertos_risc_v_trap_handler(void);
extern void freertos_vector_table(void);

/*-----------------------------------------------------------*/

#ifdef TARGET_HAS_CLIC
    #include <baremetal/clic.h>
    #include <baremetal/mem_barrier.h>

extern void freertos_risc_v_mtimer_interrupt_handler(void);

static void (*mtvt_table[TARGET_CLIC_NUM_INPUTS])(void) __attribute__((aligned(64))) = {0};
static bm_clic_t *clic;
#endif

/*-----------------------------------------------------------*/

extern void bm_ext_irq_handler(void);

/* Functions used just for breakpointing, in the debugger add breakpoint on all
 * the breakpoint_me*() functions */
volatile int breakpoint_me_data;

void __attribute__((noinline)) breakpoint_me1(uint32_t data)
{
    breakpoint_me_data = data; /* Needed to prevent this function being optimised out */
}

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
    bm_interrupt_enable_source(BM_PRIV_MODE_MACHINE, BM_INTERRUPT_MEIP);
    #endif
}
#endif

/*-----------------------------------------------------------*/

static osMutexId_t mutexSendString;

/* DO NOT CALL FROM AN ISR as this is using a mutex */
void vSendString(const char *s)
{
    osStatus_t status;

    status = osMutexAcquire(mutexSendString, osWaitForever);

    if (status == osOK)
    {
#if UART_LOCAL_USE
        /* Use write_line() as baremetal-examples's printf(), which calls _write(), self initialises
         * the UART (syscalls/sys_uart.c) */
        write_line(s);
        write_line("\r\n");

#else
        /* BareMetal SYS UART _write() (used by printf()) self initialises, so you can just use printf
         * after setting up bm_interrupt_tvec_setup() */
        printf("%s\r\n", s);
#endif

        osMutexRelease(mutexSendString);
    }
    else
    {
        breakpoint_me1(status);
    }
}

/*-----------------------------------------------------------*/

static void prvFlashLEDsTask(void *argument)
{
    /* Remove compiler warning about unused parameter. */
    (void)argument;

    int f = 1;

    vSendString("======================================================");
    vSendString("FreeRTOS Version " tskKERNEL_VERSION_NUMBER "");
    vSendString("BareMetal Examples Version " BUILD_VERSION "\n");

    vSendString("CMSIS-RTOS2-FreeRTOS multi-task example running\n");
    vSendString("3 tasks outputting text with 1 also flashing LEDs.");
    vSendString("======================================================\n");

    vSendString("This (Fx) task is the BareMetal GPIO demo in a RTX/FreeRTOS task");
    vSendString("with an additional counting number output.");
    vSendString("Feel free to flip switches 1-3, and observe blinking LEDs.\n");

    vSendString("The other (Tx/Rx) tasks demostrate a message queue.\n");

    bm_gpio_t *gpio = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);

    bool light = false;

    for (;;)
    {
        char buf[40];

        sprintf(buf, "%d: %s: %d", xGetCoreID(), osThreadGetName(osThreadGetId()), f);
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
        osDelay(250);
    }
}

/*-----------------------------------------------------------*/

void system_init(void)
{
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
    mtvt_table[1] = freertos_risc_v_mtimer_interrupt_handler; /* CLINT mtimer (IRQ 1) vector entry */

    /* Setup any remaining interrupt vectors here, e.g.:
     * mtvt_table[2 upwards] = your_isr_handler_X; */

    bm_csr_write(BM_CSR_MTVT, (xlen_t)mtvt_table);

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
        // __asm__ volatile ( "csrw mtvec, %0" : : "r" ( freertos_risc_v_trap_handler ) );
        bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE,
                                (xlen_t)freertos_risc_v_trap_handler,
                                BM_INTERRUPT_MODE_DIRECT);
    }
    #else
    {
        // __asm__ volatile ( "csrw mtvec, %0" : : "r" ( ( uintptr_t ) freertos_vector_table | 0x1 ) );
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
}

/*---------------------------------------------------------------------------
 * Application thread initialisation
 *---------------------------------------------------------------------------*/
static void app_init(void)
{
    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.name = "Fx";

    osThreadNew(prvFlashLEDsTask, NULL, &attr);
}

/*-----------------------------------------------------------*/

extern int Init_MsgQueue(void);

int main(void)
{
    int ret = 0;

    /* Initialise */
    system_init();
    osKernelInitialize(); // Initialize CMSIS-RTOS2

    /* Create a mutex type semaphore for vSendString(). */
    mutexSendString = osMutexNew(NULL); /* NULL == use default values */

    if (mutexSendString != NULL)
    {
        /* Setup threads */
        app_init();
        Init_MsgQueue();

        /* Start kernel and threads */
        osKernelStart(); // Start thread execution
    }

    return ret;
}

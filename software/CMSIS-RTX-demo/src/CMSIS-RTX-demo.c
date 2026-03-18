/* Copyright 2023-2025 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <string.h>

/* Codasip BareMetal includes */
#include "core_rv32_clic.h"
#include "rtx_os.h" /* For osRtxKernelId version string */

#include <baremetal/gpio.h>
#include <baremetal/interrupt.h>
#include <baremetal/mp.h>
#include <baremetal/platform.h>
#include <baremetal/uart.h>
#include <tiny_printf/printf.h>

/* CMSIS-RTOS2 Wrapper API */
#include "cmsis_os2.h"

/* UART Selection */
#define UART_LOCAL_USE 1 /* 1 = UART setup in this file; 0 = Use SYS UART (via printf) */

/* Local UART IRQ enable */
#define UART_LOCAL_IRQ_ENABLE \
    1 /* If UART_LOCAL_USE == 1, then use IRQ when = 1 (via NVIC API CLIC Wrapper) */

#define UART_LOCAL_IRQ_VECTORED \
    1 /* If UART_LOCAL_USE == 1, then when = 1 use vectored ISR uart_interrupt_handler()
                                         otherwise use the non-vectored handler rtx_risc_v_interrupt_handler() */

#ifdef UART_LOCAL_USE
    #include "rtx_uart.h"
#endif

static uint32_t NVIC_PriorityGroup = 0; /* NVIC Priority Group */

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
        rtx_uart_transmit_byte(uart, *str++);
    }
}
#endif

/*-----------------------------------------------------------*/

/**
 * \brief Interrupt handler for UART
 */
#if UART_LOCAL_USE && UART_LOCAL_IRQ_ENABLE && UART_LOCAL_IRQ_VECTORED
void __attribute__((interrupt)) uart_interrupt_handler(void)
{
    rtx_uart_handle_irq(uart);
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

    #if UART_LOCAL_IRQ_ENABLE
    uint32_t priority;

    priority = NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                                   /* PreemptPriority */ 6,
                                   /* SubPriority */ 0);
    NVIC_SetPriorityTrig(UART_CLIC_IRQ_ID, priority, CLIC_TRIG_EDGE_POS);
        #if UART_LOCAL_IRQ_VECTORED
    NVIC_SetVector(UART_CLIC_IRQ_ID, (uint32_t)uart_interrupt_handler);
        #else
    NVIC_SetVector(UART_CLIC_IRQ_ID, (uint32_t)NULL);
        #endif
    NVIC_ClearPendingIRQ(UART_CLIC_IRQ_ID);
    #endif

    rtx_uart_init(uart, &config);
}
#endif

/*-----------------------------------------------------------*/

/* Non-vectored interrupt handler */
void rtx_risc_v_interrupt_handler(uint32_t interrupt_number)
{
    switch (interrupt_number)
    {
#if !UART_LOCAL_IRQ_VECTORED
        case UART_CLIC_IRQ_ID:
            rtx_uart_handle_irq(uart);
            break;
#endif

        default:
            break;
    }
}

/*-----------------------------------------------------------*/

#if 0
void rtx_risc_v_exception_handler(uint32_t exception_number)
{
    /* Process all exceptions here */
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
        /* Use write_line() as baremetal-examples's printf(), which calls _write(), self initializes
         * the UART (syscalls/sys_uart.c) */
        write_line(s);
        write_line("\r\n");

#else
        /* BareMetal SYS UART _write() (used by printf()) self initializes, so you can just use printf
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
    vSendString("Codasip's RISC-V port of RTX Version " osRtxKernelId "\n");
    vSendString("BareMetal Examples Version " BUILD_VERSION "\n");

    vSendString("CMSIS-RTX multi-task example running\n");
    vSendString("3 tasks outputting text with 1 also flashing LEDs.");
    vSendString("======================================================\n");

    vSendString("This (Fx) task is the BareMetal GPIO demo in a RTX task");
    vSendString("with an additional counting number output.");
    vSendString("Feel free to flip switches 1-3, and observe blinking LEDs.\n");

    vSendString("The other (Tx/Rx) tasks demonstrate a message queue.\n");

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
#ifdef TARGET_HAS_CLIC
    /* CLIC trap (interrupt and exception) handler initialization */

    extern void risc_v_trap_handler(void);

    /* Initialize clock and system here */
    /* init_system(); */

    /* CLIC trap (interrupt and exception) handler initialization.
     *
     * This needs to be setup before calling osKernelInitialize() as that is an SVC, i.e. ecall
     * in this RISC-V port, which needs the trap handler setup before calling it. */

    /* With the Codasip CLIC all interrupts and exceptions will call risc_v_trap_handler()
     * this will determine what to do, any user interrupts can be caught by implementing:
     *  rtx_risc_v_interrupt_handler(uint32_t interrupt_number) { }
     *
     * Similarly any untrapped exception can be caught by implementing:
     *  rtx_risc_v_exception_handler(uint32_t exception_number) { }
     *
     * If an interrupt is set to vectored mode, then it's up to the developer
     * to wrap their ISR correctly if it interacts with RTX. */
    codasip_clic_init(risc_v_trap_handler);

    /* Set the NVIC (CLIC) Priority Grouping */
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup);    /* Set priority group to 0:
                                                               Bit[7..1] preempt priority Bits, 
                                                               No subpriority Bits */
    NVIC_PriorityGroup = NVIC_GetPriorityGrouping(); /* Get actual priority grouping used */

#else /* !TARGET_HAS_CLIC */
    #error CMSIS-RTX and CMSIS_6 NVIC Port to RISC-V requires the Codasip CLIC
#endif /* !TARGET_HAS_CLIC */

#if UART_LOCAL_USE
    vUartInit();
#endif

    /* Global Interrupt Enable (L110 is machine mode only) */
    bm_interrupt_enable(BM_PRIV_MODE_MACHINE);
}

/*---------------------------------------------------------------------------
 * Application thread initialization
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

/*-----------------------------------------------------------*/
int main(void)
{
    int ret = 0;

    /* Initialize */
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

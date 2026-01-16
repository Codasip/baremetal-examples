
/*
 * Copyright (c) 2024 Codasip GmbH. All rights reserverd.
 * Minimal FreeRTOS + buffer overflow demo:
 *   - 4 tasks total: Status, BuggyMalloc, BuggyStack, BuggyGlobal
 *   - Each buggy "buffer" is a uint64_t, written through a uint8_t* (8-byte view)
 *   - CHERI: out-of-bounds byte write should raise exception; handler marks and suspends task
 *   - Status checks for all faults seen, deletes buggy tasks, then deletes itself
 */

#include <FreeRTOS.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <task.h>
#include <time.h>

/* Prototypes for hooks */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

/* ---- Demo constants ---- */
#define SENSITIVE_DATA UINT64_C(0x1234CAFEBABE4321)
#define FAULT_MARKER   UINT64_C(0x1c1cdeadbeef1c1c)

#define BUFFER_BYTES   8u /* uint64_t viewed as 8-byte buffer */

/* ---- Shared pointers for Status ---- */
static uint64_t *buggyMallocBuf           = NULL;
static uint64_t *buggyMallocSensitiveData = NULL;

static uint64_t *buggyStackBuf           = NULL;
static uint64_t *buggyStackSensitiveData = NULL;

/* Global "buffer word" and global "sensitive word" (separate globals) 
 * Putting the in their own section guarantees they are next to each other in memory,
 * so the overflow from one to the other is reliable. 
 */
static volatile uint64_t buggyGlobalBufWord
    __attribute__((section(".buggy_globals"), aligned(8), used)) = 0;
static volatile uint64_t buggyGlobalSensitiveWord
    __attribute__((section(".buggy_globals"), aligned(8), used)) = SENSITIVE_DATA;

/* Task handles */
static TaskHandle_t hStatus      = NULL;
static TaskHandle_t hBuggyMalloc = NULL;
static TaskHandle_t hBuggyStack  = NULL;
static TaskHandle_t hBuggyGlobal = NULL;

/* Last exception snapshot for Status printing */
static volatile uint8_t      g_last_exception_code = 0;
static volatile TaskHandle_t g_last_task           = NULL;

/* Fault bookkeeping: set in exception handler, consumed by Status */
enum { FAULT_MALLOC = 1u << 0, FAULT_STACK = 1u << 1, FAULT_GLOBAL = 1u << 2 };
static volatile uint32_t g_fault_mask = 0;

/* ---- Simple printf via vSendString ---- */
static void vPrintf(const char *fmt, ...)
{
    char    buf[80];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    vSendString(buf);
}

/* ---- ANSI escape helpers ---- */

static void printLabelPrefix(const char *name, const char *suffix)
{
    const int LABEL_WIDTH = 32; // characters from start of label to colon
    char      label[64];
    snprintf(label, sizeof(label), "  - %s %s:", name, suffix);
    int len = (int)strlen(label);
    int pad = LABEL_WIDTH - len;
    if (pad < 1)
        pad = 1;

    /* Print label and required padding spaces using vPrintf (no putchar). */
    vPrintf("%s%*s", label, pad, "");
}

static void vMoveCursorUp(int n)
{
    for (int i = 0; i < n; i++)
    {
        vSendString("\x1b[1A"); // move cursor up 1 line
    }
}

static void vMoveCursorDown(int n)
{
    for (int i = 0; i < n; i++)
    {
        vSendString("\x1b[1B"); // move cursor down 1 line
    }
}

static void vPrintTaskStatus(TaskHandle_t t, uint64_t *buf, uint64_t *sensitive_data)
{
    const char *name = pcTaskGetName(t);

    // Status line
    printLabelPrefix(name, "Status");
    vPrintf("%s\n",
            eTaskGetState(t) >= eSuspended ? "\x1b[31mSuspended\x1b[0m" : "\x1b[32mRunning\x1b[0m");

    printLabelPrefix(name, "Buffer");
    vPrintf("0x%016" PRIx64 "\n", *buf);

    printLabelPrefix(name, "Sensitive Data");
    if (*sensitive_data == SENSITIVE_DATA)
    {
        vSendString("\x1b[32m");
    }
    else if (*sensitive_data == FAULT_MARKER)
    {
        vSendString("\x1b[93m");
    }
    else
    {
        vSendString("\x1b[31m");
    }
    vPrintf("0x%016" PRIx64 "\x1b[0m\n", *sensitive_data);
    vSendString("\n");
}

/* ---- Status task ---- */
bool g_exception_occurred = false;

static void vStatusTask(void *pvParameters)
{
    (void)pvParameters;
    bool           clear_status      = false;
    bool           all_tasks_faulted = false;
    static uint8_t exception_count   = 0;

    vSendString("\nStatus Task:\n");

    for (;;)
    {
        if (g_exception_occurred)
        {
            vMoveCursorDown(exception_count * 2);
            exception_count++;
            vSendString("[Status] Exception occurred!\n");
            vPrintf("[Status] Task: %s. Code: 0x%02x\n",
                    pcTaskGetName(g_last_task),
                    g_last_exception_code);
            vMoveCursorUp(exception_count * 2);
            g_exception_occurred = false;
        }
        if (clear_status)
        {
            vMoveCursorUp(12); // clear previous status output
        }
        clear_status = true;

        vPrintTaskStatus(hBuggyMalloc, buggyMallocBuf, buggyMallocSensitiveData);
        vPrintTaskStatus(hBuggyStack, buggyStackBuf, buggyStackSensitiveData);
        vPrintTaskStatus(hBuggyGlobal,
                         (uint64_t *)&buggyGlobalBufWord,
                         (uint64_t *)&buggyGlobalSensitiveWord);

        if (all_tasks_faulted)
        {
            vMoveCursorDown(6);
            vSendString("[Status] All buggy tasks faulted; \n");

            vTaskDelete(hBuggyMalloc);
            buggyMallocBuf = buggyMallocSensitiveData = NULL;
            hBuggyMalloc                              = NULL;

            vTaskDelete(hBuggyStack);
            buggyStackBuf = buggyStackSensitiveData = NULL;
            hBuggyStack                             = NULL;

            vTaskDelete(hBuggyGlobal);
            hBuggyGlobal = NULL;

            vTaskDelete(NULL); /* delete Status */
        }

        /* If all three buggy tasks have faulted, clean up and terminate Status */
        if ((g_fault_mask & (FAULT_MALLOC | FAULT_STACK | FAULT_GLOBAL)) ==
            (FAULT_MALLOC | FAULT_STACK | FAULT_GLOBAL))
        {
            vTaskSuspend(hBuggyMalloc);
            vTaskSuspend(hBuggyStack);
            vTaskSuspend(hBuggyGlobal);
            all_tasks_faulted = true;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ---- Buggy malloc task ---- */
static void vBuggyMallocTask(void *pvParameters)
{
    (void)pvParameters;
    uint8_t index = 0;

    buggyMallocBuf           = (uint64_t *)pvPortMalloc(sizeof(uint64_t)); /* 8 bytes */
    buggyMallocSensitiveData = (uint64_t *)pvPortMalloc(sizeof(uint64_t)); /* separate allocation */

    if (!buggyMallocBuf || !buggyMallocSensitiveData)
    {
        vPrintf("[BuggyMalloc] pvPortMalloc failed! free heap=%u\n", (unsigned)xPortGetFreeHeapSize());
        vTaskDelete(NULL);
    }

    *buggyMallocBuf           = 0;
    *buggyMallocSensitiveData = SENSITIVE_DATA;

    uint8_t *buf = (uint8_t *)buggyMallocBuf;

    for (;;)
    {
        if (g_fault_mask & FAULT_MALLOC)
        {
            vTaskSuspend(hBuggyMalloc);
        }
        buf[index] = index; /* CHERI should fault at index==8; non-CHERI corrupts heap metadata/etc. */
        vTaskDelay(pdMS_TO_TICKS(1600));
        index++;
    }
}

/* ---- Buggy stack task ---- */
static void vBuggyStackTask(void *pvParameters)
{
    (void)pvParameters;
    uint8_t index = 0;

    /* Two separate locals; adjacency is compiler-dependent */
    uint64_t sensitiveWord = SENSITIVE_DATA;
    uint64_t bufWord       = 0;

    buggyStackBuf           = &bufWord;
    buggyStackSensitiveData = &sensitiveWord;

    uint8_t *buf = (uint8_t *)&bufWord;
    for (;;)
    {
        if (g_fault_mask & FAULT_STACK)
        {
            vTaskSuspend(hBuggyStack);
            hBuggyStack = NULL;
        }
        buf[index] = index; /* CHERI should fault at index==8; non-CHERI may clobber nearby stack */
        vTaskDelay(pdMS_TO_TICKS(1200));
        index++;
    }
}

/* ---- Buggy global task ---- */
static void vBuggyGlobalTask(void *pvParameters)
{
    (void)pvParameters;
    uint8_t index            = 0;
    buggyGlobalBufWord       = 0;
    buggyGlobalSensitiveWord = SENSITIVE_DATA;

    uint8_t *buf = (uint8_t *)&buggyGlobalBufWord;

    for (;;)
    {
        if (g_fault_mask & FAULT_GLOBAL)
        {
            vTaskSuspend(hBuggyGlobal);
        }
        buf[index] = index; /* CHERI should fault at index==8; non-CHERI may hit adjacent globals if placed so */
        vTaskDelay(pdMS_TO_TICKS(1400));
        index++;
    }
}

/* ---- Exception handler ---- */
void __attribute__((aligned(64)))
freertos_risc_v_application_exception_handler(size_t mcause, uintptr_t mepc)
{
    g_exception_occurred        = true;
    TaskHandle_t t              = xTaskGetCurrentTaskHandle();
    uint8_t      exception_code = (uint8_t)(mcause & 0xff);

    g_last_exception_code = exception_code;
    g_last_task           = t;
    if (t == hBuggyMalloc && buggyMallocSensitiveData)
    {
        *buggyMallocSensitiveData = FAULT_MARKER;
        g_fault_mask |= FAULT_MALLOC;
    }
    else if (t == hBuggyStack && buggyStackSensitiveData)
    {
        *buggyStackSensitiveData = FAULT_MARKER;
        g_fault_mask |= FAULT_STACK;
    }
    else if (t == hBuggyGlobal)
    {
        buggyGlobalSensitiveWord = FAULT_MARKER;
        g_fault_mask |= FAULT_GLOBAL;
    }
    return;
}

extern void freertos_risc_v_trap_handler(void);
/* ---- Demo entry ---- */
int main_buggy(void)
{
    const uint32_t stack_words = (2U * configMINIMAL_STACK_SIZE);
    xTaskCreate(vStatusTask, "Status", stack_words, NULL, 1, &hStatus);
    xTaskCreate(vBuggyMallocTask, "BuggyMalloc", stack_words, NULL, 2, &hBuggyMalloc);
    xTaskCreate(vBuggyStackTask, "BuggyStack", stack_words, NULL, 2, &hBuggyStack);
    xTaskCreate(vBuggyGlobalTask, "BuggyGlobal", stack_words, NULL, 2, &hBuggyGlobal);

    vTaskStartScheduler();

    vSendString("vTaskStartScheduler RETURNED!\n");
    configASSERT(!"Scheduler returned");
    for (;;)
    {}
}

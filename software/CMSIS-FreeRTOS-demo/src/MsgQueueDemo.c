/*
 * Copyright (c) 2009-2023 Arm Limited. All rights reserved.
 * Copyright (c) 2024 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Code based on: https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__Message.html#gaa515fc8b956f721a8f72b2c505813bfc */

#include "cmsis_os2.h" // CMSIS RTOS header file

#include <string.h>

extern void vSendString(const char *s);

/*----------------------------------------------------------------------------
 *      Message Queue creation & usage
 *---------------------------------------------------------------------------*/

#define MSGQUEUE_OBJECTS 16 // number of Message Queue Objects

typedef struct { // object data type
    char Str[4];
} MSGQUEUE_OBJ_t;

osMessageQueueId_t mid_MsgQueue; // message queue id

osThreadId_t tid_Thread_MsgQueuePut; // thread id Put
osThreadId_t tid_Thread_MsgQueueGet; // thread id Get

void Thread_MsgQueuePut(void *argument); // thread function put in to message queue
void Thread_MsgQueueGet(void *argument); // thread function get from message queue

int Init_MsgQueue(void)
{
    mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
    if (mid_MsgQueue == NULL)
    {
        ; // Message Queue object not created, handle failure
    }

    tid_Thread_MsgQueuePut = osThreadNew(Thread_MsgQueuePut, NULL, NULL);
    if (tid_Thread_MsgQueuePut == NULL)
    {
        return (-1);
    }
    tid_Thread_MsgQueueGet = osThreadNew(Thread_MsgQueueGet, NULL, NULL);
    if (tid_Thread_MsgQueueGet == NULL)
    {
        return (-1);
    }

    return (0);
}

void Thread_MsgQueuePut(void *argument)
{
    MSGQUEUE_OBJ_t msg;
    (void)argument;

    char str[32];

    msg.Str[0] = 'A'; // do some work...
    msg.Str[1] = '\0';

    while (1)
    {
        strcpy(str, "Sending ");
        strcat(str, &msg.Str[0]);
        vSendString(str);

        osMessageQueuePut(mid_MsgQueue, &msg, 0U, 0U);
        osDelay(1000); /* Delay 1 second */

        /* Create new data to put */
        if (msg.Str[0] == 'Z')
            msg.Str[0] = 'A';
        else
            msg.Str[0]++;
    }
}

void Thread_MsgQueueGet(void *argument)
{
    MSGQUEUE_OBJ_t msg;
    osStatus_t     status;
    (void)argument;

    char str[32];

    while (1)
    {
        status = osMessageQueueGet(mid_MsgQueue, &msg, NULL, 0U); // wait for message
        if (status == osOK)
        {
            strcpy(str, "Received ");
            strcat(str, &msg.Str[0]);
            vSendString(str);
        }
    }
}

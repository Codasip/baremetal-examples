/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#define TARGET_PLATFORM_NAME        "DoomBar"

#define TARGET_TIMER_ADDR           CLINT_ADDR /* This is required for FreeRTOS support */

#define TARGET_PLATFORM_FREQ        TARGET_CLK_FREQ

#define TARGET_GPIO_PORT_LED0       0
#define TARGET_GPIO_PORT_LED1       1
#define TARGET_GPIO_PORT_LED2       2
#define TARGET_GPIO_PORT_LED3       3
#define TARGET_GPIO_PORT_LED4       4
#define TARGET_GPIO_PORT_LED5       5
#define TARGET_GPIO_PORT_LED6       6
#define TARGET_GPIO_PORT_LED7       7

#define TARGET_GPIO_PORT_SWITCH0    0
#define TARGET_GPIO_PORT_SWITCH1    1
#define TARGET_GPIO_PORT_SWITCH2    2
#define TARGET_GPIO_PORT_SWITCH3    3
#define TARGET_GPIO_PORT_SWITCH4    4
#define TARGET_GPIO_PORT_SWITCH5    5
#define TARGET_GPIO_PORT_SWITCH6    6
#define TARGET_GPIO_PORT_SWITCH7    7

#define TARGET_GPIO_SD_CARD_DETECT  0
#define TARGET_GPIO_SD_POWER_ENABLE 1
#define TARGET_GPIO_SD_FAST_CLOCK   2

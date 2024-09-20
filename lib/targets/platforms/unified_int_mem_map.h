/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#define CLIC_ADDR        0x00040000
#define PLIC_ADDR        0x40000000
#define PLAT_ID_ADDR     0x60000000
#define CLINT_ADDR       0x60010000
#define ACLINT_ADDR      0x60014000
#define UART_ADDR        0x60100000
#define SPI_FLASH_ADDR   0x60200000
#define SPI_SD_ADDR      0x60210000
#define GPIO_IO_ADDR     0x60300000
#define GPIO_SD_ADDR     0x60310000
#define I2C_PWR_ADDR     0x60400000
#define TRNG_ADDR        0x60510000
#define AEAD_ADDR        0x60550000

#define UART_IRQ_ID      0
#define SPI_FLASH_IRQ_ID 2
#define SPI_SD_IRQ_ID    3
#define I2C_PWR_IRQ_ID   4
#define GPIO_IO_IRQ_ID   6
#define GPIO_SD_IRQ_ID   7

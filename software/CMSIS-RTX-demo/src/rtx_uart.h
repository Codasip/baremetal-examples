/* Copyright 2023-2025 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

/* Copied and modified from lib/include/baremental/uart.h */

#ifndef __RTX_UART_H__
#define __RTX_UART_H__

#include "baremetal/common.h"
#include "baremetal/interrupt.h"
#include "baremetal/uart.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 16550 UART */

/**
 * \brief Function to initialize the UART peripheral
 *
 * \param uart Pointer to the UART driver structure linked to the peripheral to initialize.
 * \param config UART configuration
 */
void rtx_uart_init(bm_uart_t *uart, const bm_uart_config_t *config);

/**
 * \brief Function to transmit a single byte of data over UART
 *
 * \param uart Pointer to the UART driver structure
 *
 * \param byte Data to transmit
 */
void rtx_uart_transmit_byte(bm_uart_t *uart, uint8_t byte);

/**
 * \brief Function to receive a single byte of data over UART
 *
 * \param uart Pointer to the UART driver structure
 *
 * \returns Received data, or -1 if none available
 */
int rtx_uart_receive_byte(bm_uart_t *uart);

/**
 * \brief Flush internal buffer, block untill all characters transmit
 *
 * \param uart Pointer to the UART driver structure to flush
 */
void rtx_uart_flush(bm_uart_t *uart);

/**
 * \brief Handle UART interrupt
 *
 * \param uart Pointer to UART structure
 *
 * \return 0 if successful, non-zero value in case of error
 */
int rtx_uart_handle_irq(bm_uart_t *uart);

#endif /* __RTX_UART_H__ */

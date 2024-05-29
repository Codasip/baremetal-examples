/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_UART_H
#define BAREMETAL_UART_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UART_FIFO_SIZE 128

/** \brief Structure describing UART peripheral registers */
typedef struct bm_uart_regs bm_uart_regs_t;

/** \brief FIFO structure */
typedef struct bm_uart_fifo {
    volatile size_t  start;
    volatile size_t  pos;
    volatile size_t  size;
    volatile uint8_t buffer[UART_FIFO_SIZE];
} bm_uart_fifo_t;

/** \brief Data format options */
typedef enum {
    BM_UART_DATA_BITS_5 = 0,
    BM_UART_DATA_BITS_6 = 1,
    BM_UART_DATA_BITS_7 = 2,
    BM_UART_DATA_BITS_8 = 3,
} bm_uart_data_bits_t;

/** \brief Parity options */
typedef enum {
    BM_UART_PARITY_NONE    = 0 << 3,
    BM_UART_PARITY_ODD     = 1 << 3,
    BM_UART_PARITY_EVEN    = 3 << 3,
    BM_UART_PARITY_FORCED1 = 5 << 3,
    BM_UART_PARITY_FORCED0 = 7 << 3,
} bm_uart_parity_t;

/** \brief Stop bit options */
typedef enum {
    BM_UART_STOP_BITS_1 = 0 << 2,
    BM_UART_STOP_BITS_2 = 1 << 2,
} bm_uart_stop_bits_t;

/** \brief Structure holding data neccessary to service a UART device. */
typedef struct {
    bm_uart_regs_t *regs;       ///< Pointer to the peripheral registers
    unsigned        ext_irq_id; ///< External interrupt identifier
    unsigned        freq;       ///< Clock frequency
    bm_uart_fifo_t  rx;         ///< Software receive fifo
    bm_uart_fifo_t  tx;         ///< Software transfer fifo
    bool            use_irq;    ///< Interrupt configuration
} bm_uart_t;

/** \brief UART device configuration */
typedef struct {
    unsigned            baud_rate;   ///< Baud rate
    bm_uart_data_bits_t data_format; ///< Data bits
    bm_uart_parity_t    parity;      ///< Parity configuration
    bm_uart_stop_bits_t stop;        ///< Stop bits
    bool                use_irq;     ///< Interrupt configuration

} bm_uart_config_t;

/**
 * \brief Function to initialize the UART peripheral
 *
 * \param uart Pointer to the UART driver structure linked to the peripheral to initialize.
 * \param config UART configuration
 */
void bm_uart_init(bm_uart_t *uart, const bm_uart_config_t *config);

/**
 * \brief Function to transmit a single byte of data over UART
 *
 * \param uart Pointer to the UART driver structure
 *
 * \param byte Data to transmit
 */
void bm_uart_transmit_byte(bm_uart_t *uart, uint8_t byte);

/**
 * \brief Function to receive a single byte of data over UART
 *
 * \param uart Pointer to the UART driver structure
 *
 * \returns Received data, or -1 if none available
 */
int bm_uart_receive_byte(bm_uart_t *uart);

/**
 * \brief Flush internal buffer, block untill all characters transmit
 *
 * \param uart Pointer to the UART driver structure to flush
 */
void bm_uart_flush(bm_uart_t *uart);

/**
 * \brief Handle UART interrupt
 *
 * \param uart Pointer to UART structure
 *
 * \return 0 if successful, non-zero value in case of error
 */
int bm_uart_handle_irq(bm_uart_t *uart);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_UART_H */

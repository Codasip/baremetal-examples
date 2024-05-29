/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/uart.h"

#include "baremetal/common.h"
#include "baremetal/interrupt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LSR_TEMT    0x40
#define LSR_THRE    0x20
#define LSR_DR      0x1

#define IIR_INTPEND 0x1

#define IER_ELSI    0x4
#define IER_ERBEI   0x2
#define IER_ERBFI   0x1

#define LCR_DLA     0x80

#define FCR_TL_14   0xC0
#define FCR_TL_8    0x80
#define FCR_TL_4    0x40
#define FCR_TL_1    0x00
#define FCR_XFR     0x4
#define FCR_RFR     0x2
#define FCR_FIFOEN  0x1

struct bm_uart_regs {
    uint8_t _reserved1[0x1000]; ///< Codasip UART uses an offset 0x1000
    union {
        const volatile uint32_t RBR; ///< (@ 0x0000) Receiver Buffer Register
        volatile uint32_t       THR; ///< (@ 0x0000) Transmiter Holding Register
        volatile uint32_t       DLL; ///< (@ 0x0000) Divisor Latch Register (LSB)
    };
    union {
        volatile uint32_t IER; ///< (@ 0x0004) Interrupt Enable Register
        volatile uint32_t DLM; ///< (@ 0x0004) Divisor Latch Register (MSB)
    };
    union {
        const volatile uint32_t IIR; ///< (@ 0x0008) Interrupt Identification Register
        volatile uint32_t       FCR; ///< (@ 0x0008) Fifo Control Register
    };
    volatile uint32_t LCR; ///< (@ 0x000C) Line Control Register
    volatile uint32_t MCR; ///< (@ 0x0010) Modem Control Register
    volatile uint32_t LSR; ///< (@ 0x0014) Line Status Register
    volatile uint32_t MSR; ///< (@ 0x0018) Modem Status Register
    volatile uint32_t SCR; ///< (@ 0x001C) Scratch Register
};

/**
 * \brief Function for initialization of fifo structure
 *
 * \param f Pointer to the structure to initialize
 */
static void fifo_init(bm_uart_fifo_t *f)
{
    f->start = 0;
    f->pos   = 0;
    f->size  = UART_FIFO_SIZE;
}

/**
 * \brief Function for checking whether fifo is empty
 *
 * \param f Pointer to the fifo structure
 *
 * \return True if the fifo is empty, false otherwise
 */
static int fifo_empty(const bm_uart_fifo_t *f)
{
    return f->start == f->pos;
}

/**
 * \brief Function for checking whether fifo is full
 *
 * \param f Pointer to the structure of registers of the peripheral
 *
 * \return True if the fifo is full, false otherwise
 */
static int fifo_full(const bm_uart_fifo_t *f)
{
    return f->start == ((f->pos + 1) % f->size);
}

/**
 * \brief Function for adding a single byte of data to the fifo
 *
 * \param f Pointer to the fifo structure
 * \param byte Data to add
 *
 * \return 0 on success, -1 otherwise
 */
static int fifo_push(bm_uart_fifo_t *f, uint8_t byte)
{
    if (fifo_full(f))
    {
        return -1;
    }

    f->buffer[f->pos] = byte;
    f->pos            = (f->pos + 1) % f->size;

    return 0;
}

/**
 * \brief Function for getting a single byte of data from the fifo
 *
 * \param f Pointer to the fifo structure
 *
 * \return Retrieved data on success, -1 otherwise
 */
static int fifo_pop(bm_uart_fifo_t *f)
{
    if (fifo_empty(f))
    {
        return -1;
    }

    uint8_t byte = f->buffer[f->start];
    f->start     = (f->start + 1) % f->size;

    return byte;
}

/**
 * \brief Function to transmit internal buffer of UART driver structure,
 * over the linked UART controller device
 *
 * \param uart Pointer to the UART driver structure
 */
static inline void fill_tx_fifo(bm_uart_t *uart)
{
    for (int i = 0; i < 16 && !fifo_empty(&uart->tx); ++i)
    {
        uart->regs->THR = fifo_pop(&uart->tx);
    }
}

/**
 * \brief Function to fill internal buffer of UART driver structure,
 * with data received from the linked UART controller device
 *
 * \param uart Pointer to the UART driver structure
 */
static inline void clean_rx_fifo(bm_uart_t *uart)
{
    while (uart->regs->LSR & LSR_DR)
    {
        uint8_t byte = uart->regs->RBR;

        // Drop new data in case of internal buffer overrun
        if (!fifo_full(&uart->rx))
        {
            fifo_push(&uart->rx, byte);
        }
    }
}

void bm_uart_init(bm_uart_t *uart, const bm_uart_config_t *config)
{
    // Initialize internal buffers
    fifo_init(&uart->rx);
    fifo_init(&uart->tx);

    // Calculate the clock divisor
    unsigned divisor = (uart->freq + 8 * config->baud_rate) / (16 * config->baud_rate);

    // The divisor must be 2 or more
    if (divisor < 2)
    {
        divisor = 2;
    }

    // Set divisor access latch = 1
    uart->regs->LCR = LCR_DLA;

    // Set the divisor
    uart->regs->DLL = divisor & 0xff;
    uart->regs->DLM = (divisor & 0xff00) >> 8;

    // Set data format. Resets divisor access latch
    uart->regs->LCR = config->data_format | config->parity | config->stop;

    // Enable and reset FIFOs, set receiver trigger level to 4 bytes
    uart->regs->FCR = FCR_TL_4 | FCR_XFR | FCR_RFR | FCR_FIFOEN;

    uart->use_irq = config->use_irq;

    if (uart->use_irq)
    {
        // Enable interrupts
        bm_ext_irq_enable(uart->ext_irq_id);
        uart->regs->IER = IER_ELSI | IER_ERBEI | IER_ERBFI;
    }
}

void bm_uart_transmit_byte(bm_uart_t *uart, uint8_t byte)
{
    if (!uart->use_irq)
    {
        // Block until uart is ready to tx
        while (!(uart->regs->LSR & LSR_THRE))
            ;

        // Initiate transmission of the next byte
        uart->regs->THR = byte;
        return;
    }

    // Block until some characters transmit
    while (fifo_full(&uart->tx))
        ;

    bm_ext_irq_disable(uart->ext_irq_id);
    fifo_push(&uart->tx, byte);

    if (fifo_full(&uart->tx) && (uart->regs->LSR & LSR_THRE))
    {
        // Flush internal FIFO in case its full
        fill_tx_fifo(uart);
    }
    bm_ext_irq_enable(uart->ext_irq_id);
}

int bm_uart_receive_byte(bm_uart_t *uart)
{
    if (!uart->use_irq)
    {
        // No data available
        if (!(uart->regs->LSR & LSR_DR))
        {
            return -1;
        }

        // Get the character
        return uart->regs->RBR;
    }

    // No data available
    if (fifo_empty(&uart->rx))
    {
        return -1;
    }

    int ret;
    bm_ext_irq_disable(uart->ext_irq_id);
    ret = fifo_pop(&uart->rx);
    bm_ext_irq_enable(uart->ext_irq_id);

    // Get the character
    return ret;
}

void bm_uart_flush(bm_uart_t *uart)
{
    if (uart->use_irq)
    {
        bm_ext_irq_disable(uart->ext_irq_id);

        // Start flush of the internal buffer
        if (!fifo_empty(&uart->tx))
        {
            while (!(uart->regs->LSR & LSR_THRE))
                ;
            fill_tx_fifo(uart);
        }

        bm_ext_irq_enable(uart->ext_irq_id);

        // Wait for the flush to finish
        while (!fifo_empty(&uart->tx))
            ;
    }

    // Wait for characters to transmit
    while (!(uart->regs->LSR & LSR_TEMT))
        ;
}

int bm_uart_handle_irq(bm_uart_t *uart)
{
    // Check interrupt pending
    if (uart->regs->IIR & IIR_INTPEND)
    {
        return -1;
    }

    if (uart->regs->LSR & LSR_THRE)
    {
        fill_tx_fifo(uart);
    }

    clean_rx_fifo(uart);

    return 0;
}

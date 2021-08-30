/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_polarfire
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"

#include "periph/uart.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Allocate memory to store the callback functions & buffers
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

/**
 * @brief   Get the pointer to the base register of the given UART device
 *
 * @param[in] dev       UART device identifier
 *
 * @return              base register address
 */
static inline SercomUsart *dev(uart_t dev)
{
    return uart_config[dev].dev;
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

 

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{

}

void uart_poweron(uart_t uart)
{
    (void)dev;
}

void uart_poweroff(uart_t uart)
{
    (void)dev;
}

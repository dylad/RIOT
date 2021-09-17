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
#include "board.h"
#include "periph_cpu.h"
#include "periph/gpio.h"
#include "periph/uart.h"

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
static inline MSS_UART_TypeDef *dev(uart_t dev)
{
    return uart_config[dev].dev;
}

void rx_handler(mss_uart_instance_t* this_uart)
{
    (void)this_uart;
    uint8_t byte;

    LED3_OFF;
    MSS_UART_get_rx(&g_mss_uart0_lo, &byte, 1);
    uart_ctx[0].rx_cb(uart_ctx[0].arg, byte);
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    (void) baudrate;
    (void) uart;
    LED0_OFF;
    mss_config_clk_rst(MSS_PERIPH_MMUART0, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);

    MSS_UART_init(&g_mss_uart0_lo, MSS_UART_115200_BAUD,
                  MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);

    if (rx_cb) {
        LED1_OFF;
        uart_ctx[0].rx_cb = rx_cb;
        uart_ctx[0].arg = arg;
        MSS_UART_set_rx_handler(&g_mss_uart0_lo, rx_handler, MSS_UART_FIFO_SINGLE_BYTE);
        MSS_UART_enable_local_irq(&g_mss_uart0_lo);
    }
    LED2_OFF;
    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    (void)uart;

    MSS_UART_polled_tx(&g_mss_uart0_lo, data, len);
}

void uart_poweron(uart_t uart)
{
    (void)uart;
}

void uart_poweroff(uart_t uart)
{
    (void)uart;
}

void uart_isr(int num)
{
    (void)num;
    LED3_OFF;
}

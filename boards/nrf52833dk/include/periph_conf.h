/*
 * Copyright (C) 2016-2018 Freie Universität Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52833dk
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF52833 DK
 *
 * @author      Thomas POTIER <thomas.potier@u-psud.fr>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPI0,
        .sclk = GPIO_PIN(0, 19),
        .mosi = GPIO_PIN(1, 5),
        .miso = GPIO_PIN(0, 25),
    }
};
#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    { /* Mapped to Arduino D0/D1 pins */
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 24),
        .tx_pin     = GPIO_PIN(0, 22),
        .rts_pin    = (uint8_t)GPIO_UNDEF,
        .cts_pin    = (uint8_t)GPIO_UNDEF,
        .irqn       = UARTE1_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)
#define UART_1_ISR          (isr_uarte1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */

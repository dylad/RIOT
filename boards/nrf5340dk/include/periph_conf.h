/*
 * Copyright (C) 2023 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    boards_nrf5340dk
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF5340DK
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = NRF_TIMER0_S,
        .channels = 5,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER0_IRQn
    },
    {
        .dev      = NRF_TIMER1_S,
        .channels = 5,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER1_IRQn
    },
};

#define TIMER_0_ISR         isr_timer0 /**< Timer0 IRQ*/
#define TIMER_1_ISR         isr_timer1 /**< Timer1 IRQ */

#define TIMER_NUMOF         ARRAY_SIZE(timer_config) /**< Timer configuration NUMOF */
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0_S,
        .rx_pin     = GPIO_PIN(1, 0),
        .tx_pin     = GPIO_PIN(1, 1),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = SERIAL0_IRQn,
    },
};

#define UART_0_ISR          (isr_serial0) /**< SERIAL0_IRQn */

#define UART_NUMOF          ARRAY_SIZE(uart_config) /**< UART configuration NUMOF */
/** @} */

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos, dev */
    { .psel = SAADC_CH_PSELP_PSELP_AnalogInput0, .pol  = ADC_PSEL_POS, .mode = ADC_SINGLE_ENDED},
};

#define ADC_NUMOF                           ARRAY_SIZE(adc_channels)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */

/*
 * Copyright (C) 2025 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam4s
 * @{
 *
 * @file
 * @brief       CPU specific definitions for internal peripheral handling
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN
#define HAVE_GPIO_T
typedef uint32_t gpio_t;

#define GPIO_UNDEF          (0xffffffff)

#define GPIO_PIN(x, y)      (((uint32_t)PIOA + (x << 9)) | y)
#endif /* DOXYGEN */

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @brief   We use one channel for each defined timer
 *
 * While the peripheral provides three channels, the current interrupt
 * flag handling leads to a race condition where calling timer_clear() on one
 * channel can disable a pending flag for other channels.
 * Until resolved, limit the peripheral to only one channel.
 */
#define TIMER_CHANNEL_NUMOF (1)

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 3 bit to determine the pin functions:
 * - bit 0: in/out
 * - bit 1: PU enable
 * - bit 2: OD enable
 */
#define GPIO_MODE(io, pu, od)   (io | (pu << 1) | (od << 2))

#ifndef DOXYGEN
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0),    /**< IN */
    GPIO_IN_PD = 0xf,                   /**< not supported by HW */
    GPIO_IN_PU = GPIO_MODE(0, 1, 0),    /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(1, 0, 0),    /**< OUT (push-pull) */
    GPIO_OD    = GPIO_MODE(1, 0, 1),    /**< OD */
    GPIO_OD_PU = GPIO_MODE(1, 1, 1),    /**< OD with pull-up */
} gpio_mode_t;

#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif /* ndef DOXYGEN */

/**
 * @brief Available ports on the SAM3X8E
 */
enum {
    PA = 0,                      /**< port A */
    PB = 1,                      /**< port B */
    PC = 1 << 1,                 /**< port C */
    PD = 1 << 2,                 /**< port D */
};

/**
 * @brief   GPIO mux configuration
 */
typedef enum {
    GPIO_MUX_A = 0,         /**< alternate function A */
    GPIO_MUX_B = 1,         /**< alternate function B */
    GPIO_MUX_C = 2,         /**< alternate function C */
    GPIO_MUX_D = 3,         /**< alternate function D */
} gpio_mux_t;

/**
 * @brief   Timer configuration data
 */
typedef struct {
    Tc *dev;                /**< timer device */
    uint8_t id_ch0;         /**< ID of the timer's first channel */
} timer_conf_t;

/**
 * @brief   UART configuration data
 */
typedef struct {
    Uart *dev;              /**< U(S)ART device used */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
    gpio_mux_t mux;         /**< MUX used for pins */
    uint8_t pmc_id;         /**< bit in the PMC register of the device*/
    uint8_t irqn;           /**< interrupt number of the device */
} uart_conf_t;

/**
 * @brief   Configure the given GPIO pin to be used with the given MUX setting
 *
 * @param[in] pin           GPIO pin to configure
 * @param[in] mux           MUX setting to use
 */
void gpio_init_mux(gpio_t pin, gpio_mux_t mux);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */

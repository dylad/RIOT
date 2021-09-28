/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_polarfire
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <inttypes.h>

#include "cpu.h"
#include "clic.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

typedef int GPIO_Type;
typedef int gpio_af_t;
/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

#define GPIO_PIN(x, y) ((gpio_t)(x << 5 | y))

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 2 bit to determine the pin functions:
 * - bit 0: enable output
 * - bit 1: enable input
 */
#define GPIO_MODE(ei, eo)   (eo | (ei << 1))

#ifndef DOXYGEN
/**
 * @brief   Override GPIO modes
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(1, 0),       /**< IN */
    GPIO_OUT   = GPIO_MODE(0, 1),       /**< OUT (push-pull) */
    GPIO_IN_PD = 0xfc,                  /**< IN with pull-down */
    GPIO_IN_PU = 0xfd,                  /**< IN with pull-up */
    GPIO_OD    = 0xfe,                  /**< not supported by HW */
    GPIO_OD_PU = 0xff                   /**< not supported by HW */
} gpio_mode_t;
#endif
#endif /* ndef DOXYGEN */

/** @} */

/**
 * @brief   Timer device configuration
 */
typedef struct {
    TIMER_TypeDef *dev;           /**< pointer to the used Timer device */
    unsigned num;
} timer_conf_t;

typedef struct {
    MSS_UART_TypeDef *dev;       /**< pointer to the used UART device */
} uart_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */

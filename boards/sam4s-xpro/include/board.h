/*
 * Copyright (C) 2025 Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sam4s-xpro
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Microchip SAM 4S Xplained Pro
 *              board
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_ZTIMER_USEC_WIDTH        (16)      /**< TIMER_DEV(0) is 16 bit wide */
/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PC, 23)

#define LED_PORT            PIOC
#define LED0_MASK           (PIO_PC23)

#define LED0_ON             (PIOC->PIO_CODR  = LED0_MASK)
#define LED0_OFF            (PIOC->PIO_SODR  = LED0_MASK)
#define LED0_TOGGLE         ((PIOC->PIO_ODSR & LED0_MASK) ? LED0_OFF : LED0_ON)
/** @} */

/**
 * @name SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PA, 2)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

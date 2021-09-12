/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_mpfs-icicle-kit-es
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for MPFS-Icicle-kit
 *              (engineering sample)
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

void led_init(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    led_init();
}

/**
 * @brief Initialize the boards on-board LED
 */
void led_init(void)
{
    gpio_init(LED0_PIN, 0);
    gpio_init(LED1_PIN, 0);
    gpio_init(LED2_PIN, 0);
    gpio_init(LED3_PIN, 0);

    LED0_ON;
    LED1_ON;
    LED2_ON;
    LED3_ON;
}

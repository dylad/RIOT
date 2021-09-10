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

    /* Turn on clock */
    SYSREG->SUBBLK_CLOCK_CR |= (1 << 22);
        /* Remove soft reset */
    SYSREG->SOFT_RESET_CR   &= (uint32_t)~(1 << 2);

    //GPIO2_LO->GPIO_SET_BITS |= (1 << 16);

    /* initialize the boards LEDs */
    //led_init();

    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board LED
 */
void led_init(void)
{
   /* gpio_init(LED0_PIN, 0);
    gpio_init(LED1_PIN, 0);
    gpio_init(LED2_PIN, 0);
    gpio_init(LED3_PIN, 0);

    gpio_set(LED0_PIN);
    gpio_clear(LED1_PIN);
    gpio_set(LED2_PIN);
    gpio_clear(LED3_PIN);*/
}

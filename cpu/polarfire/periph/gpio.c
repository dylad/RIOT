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
 * @brief           Polarfire GPIO implementation
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include "cpu.h"
#include "clic.h"
#include "periph_cpu.h"
#include "periph/gpio.h"

/**
 * @brief   Extract the port base address from the given pin identifier
 */
static inline GPIO_TypeDef *_port(gpio_t pin)
{
    return (GPIO_TypeDef *)(GPIO0_LO + ((pin >> 5) << 12));
}

/**
 * @brief   Extract the port number form the given identifier
 *
 * The port number is extracted by looking at bits 10, 11, 12, 13 of the base
 * register addresses.
 */
static inline int _port_num(gpio_t pin)
{
    return (((pin >> 5) & 0xf));
}

/**
 * @brief   Extract the pin number from the last 5 bits of the pin identifier
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x1f);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_TypeDef *port = _port(pin);
    unsigned pin_num = _pin_num(pin);
    
    port->GPIO_CFG[pin_num] = mode;
  
    return 0;
}

void gpio_init_af(gpio_t pin, gpio_af_t af)
{
    GPIO_TypeDef *port = _port(pin);
    unsigned pin_num = _pin_num(pin);
    (void)pin_num;
    (void)port;
    (void)af;
}

int gpio_read(gpio_t pin)
{
    GPIO_TypeDef *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    return  (port->GPIO_IN & (1 << pin_num));
}

void gpio_set(gpio_t pin)
{
    GPIO_TypeDef *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    port->GPIO_SET_BITS = ( 1 << pin_num);
}

void gpio_clear(gpio_t pin)
{
    GPIO_TypeDef *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    port->GPIO_CLR_BITS = ( 1 << pin_num);
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    }
    else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}

/** @} */

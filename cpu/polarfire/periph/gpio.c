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

#include <assert.h>
#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "plic.h"
#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef MODULE_PERIPH_GPIO_IRQ
/* Same as defined in mss_gpio.c */
#define NB_OF_GPIO_INTR (41u)

static gpio_isr_ctx_t gpio_config[NB_OF_GPIO_INTR];
extern uint8_t (*ext_irq_handler_table[PLIC_NUM_SOURCES])(void);
#endif /* MODULE_PERIPH_GPIO_IRQ */
/**
 * @brief   Extract the port base address from the given pin identifier
 */
static  GPIO_TypeDef *_port(gpio_t pin)
{
    switch (pin >> 5)
    {
        case 0:
            return GPIO0_LO;
        case 1:
            return GPIO1_LO;
        case 2:
            return GPIO2_LO;
        
        default:
            assert(0);
            return 0;
    }
}

/**
 * @brief   Extract the port number form the given identifier
 *
 * The port number is extracted by looking at bits 10, 11, 12, 13 of the base
 * register addresses.
 */
static inline unsigned _port_num(gpio_t pin)
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
    (void)pin;
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

#ifdef MODULE_PERIPH_GPIO_IRQ

static uint32_t _get_irq(gpio_t pin)
{
    uint32_t port_nb = _port_num(pin);
    uint32_t irq_line;

    switch (port_nb)
    {
        case 0: /* Fall-through */
        case 2:
            irq_line = GPIO0_BIT0_or_GPIO2_BIT0_PLIC_0;
            break;
        case 1:
            irq_line = GPIO1_BIT0_or_GPIO2_BIT14_PLIC_14;
            break;
        default:
            assert(0);
    }

    irq_line += _pin_num(pin);

    return irq_line;
}

uint8_t gpio1_bit16_or_gpio2_bit30_plic_30_IRQHandler(void)
{
    uint32_t ei_line = _get_irq(GPIO_PIN(2,30)) - GPIO0_BIT0_or_GPIO2_BIT0_PLIC_0;
    LED3_TOGGLE;
    MSS_GPIO_clear_irq(GPIO2_LO, MSS_GPIO_30);
    gpio_config[ei_line].cb(gpio_config[ei_line].arg);
    return EXT_IRQ_KEEP_ENABLED;
}

uint8_t  gpio1_bit17_or_gpio2_bit31_plic_31_IRQHandler(void)
{
    uint32_t ei_line = _get_irq(GPIO_PIN(2,31)) - GPIO0_BIT0_or_GPIO2_BIT0_PLIC_0;
    LED0_TOGGLE;
    MSS_GPIO_clear_irq(GPIO2_LO, MSS_GPIO_31);
    gpio_config[ei_line].cb(gpio_config[ei_line].arg);
    return EXT_IRQ_KEEP_ENABLED;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    GPIO_TypeDef *port = _port(pin);
    unsigned pin_num = _pin_num(pin);
    printf("port:%p, pin:%d\n", port, pin_num);
    printf("cb:%p\n", cb);
    /* Get external interrupt line */
    uint32_t ei_line = _get_irq(pin);
    printf("eiline:%d\n", ei_line - GPIO0_BIT0_or_GPIO2_BIT0_PLIC_0);
    /* Store callback and argument */
    gpio_config[ei_line - GPIO0_BIT0_or_GPIO2_BIT0_PLIC_0].cb = cb;
    gpio_config[ei_line - GPIO0_BIT0_or_GPIO2_BIT0_PLIC_0].arg = arg;
    /* Configure GPIO as interrupt */
    MSS_GPIO_config(port, pin_num, mode | flank);

    plic_set_isr_cb(ei_line, ext_irq_handler_table[ei_line]);

    MSS_GPIO_enable_irq(port, pin_num);

    plic_set_priority(ei_line, 2);

    plic_enable_interrupt(ei_line);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    (void)pin;
}
void gpio_irq_disable(gpio_t pin)
{
    (void)pin;
}

#endif /* MODULE_PERIPH_GPIO_IRQ */
/** @} */

/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2023 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_sam0_common
 * @ingroup         drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief           CPU specific part of the Peripheral GPIO Low-Level API
 *
 * @author          Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include "architecture.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

/* Provide base address of the GPIO peripheral via APB */
#if defined(PORT_SEC_REGS)
#  define GPIO_APB_BASE PORT_SEC_REGS
#else
#  define GPIO_APB_BASE PORT_REGS
#endif

/* Provide base address of the GPIO peripheral via IOBUS */
#if defined(PORT_IOBUS_SEC_REGS)
#  define GPIO_IOBUS_BASE PORT_IOBUS_SEC_REGS
#elif defined(PORT_IOBUS)
#  define GPIO_IOBUS_BASE PORT_IOBUS_REGS
#else
#  define GPIO_IOBUS_BASE GPIO_APB_BASE /* no IOBUS present, fall back to APB */
#endif

#define GPIO_PORT_NUMBERING_ALPHABETIC  1

#if PORT_GROUPS >= 1
#  define GPIO_PORT_0   ((uintptr_t)&GPIO_IOBUS_BASE->GROUP[0])
#endif
#if PORT_GROUPS >= 2
#  define GPIO_PORT_1   ((uintptr_t)&GPIO_IOBUS_BASE->GROUP[1])
#endif
#if PORT_GROUPS >= 3
#  define GPIO_PORT_2   ((uintptr_t)&GPIO_IOBUS_BASE->GROUP[2])
#endif
#if PORT_GROUPS >= 4
#  define GPIO_PORT_3   ((uintptr_t)&GPIO_IOBUS_BASE->GROUP[3])
#endif
#if PORT_GROUPS >= 5
#  define GPIO_PORT_4   ((uintptr_t)&GPIO_IOBUS_BASE->GROUP[4])
#endif
#if PORT_GROUPS >= 5
#  define GPIO_PORT_4   ((uintptr_t)&GPIO_IOBUS_BASE->GROUP[4])
#endif
#if PORT_GROUPS >= 6
#  define GPIO_PORT_5   ((uintptr_t)&GPIO_IOBUS_BASE->GROUP[5])
#endif
#if PORT_GROUPS >= 7
#  define GPIO_PORT_6   ((uintptr_t)&GPIO_IOBUS_BASE->GROUP[6])
#endif
#if PORT_GROUPS >= 8
#  define GPIO_PORT_7   ((uintptr_t)&GPIO_IOBUS_BASE->GROUP[7])
#endif

/**
 * @brief   Get a GPIO port by number
 */
#define GPIO_PORT(num)  ((uintptr_t)&GPIO_IOBUS_BASE->GROUP[(num)])

/**
 * @brief   Get a GPIO port number by gpio_port_t value
 */
#define GPIO_PORT_NUM(port) \
    (((port) - (uintptr_t)&GPIO_IOBUS_BASE->GROUP[0]) / sizeof(GPIO_IOBUS_BASE->GROUP[0]))

static inline gpio_port_t gpio_port(uword_t num)
{
    return (uintptr_t)&GPIO_IOBUS_BASE->GROUP[num];
}

static inline uword_t gpio_port_num(gpio_port_t port)
{
    return (port - (uintptr_t)&GPIO_IOBUS_BASE->GROUP[0]) / sizeof(GPIO_IOBUS_BASE->GROUP[0]);
}

static inline port_group_registers_t *sam0_gpio_iobus2ap(port_group_registers_t *iobus)
{
    const uintptr_t iobus_base = (uintptr_t)GPIO_IOBUS_BASE;
    const uintptr_t apb_base = (uintptr_t)GPIO_APB_BASE;

    return (port_group_registers_t *)((uintptr_t)iobus - (iobus_base - apb_base));
}

static inline uword_t gpio_ll_read(gpio_port_t port)
{
    port_group_registers_t *p = (port_group_registers_t *)port;
    if (!IS_USED(MODULE_PERIPH_GPIO_FAST_READ)) {
        p = sam0_gpio_iobus2ap(p);
    }
    return p->PORT_IN;
}

static inline uword_t gpio_ll_read_output(gpio_port_t port)
{
    port_group_registers_t *p = (port_group_registers_t *)port;
    return p->PORT_OUT;
}

static inline void gpio_ll_set(gpio_port_t port, uword_t mask)
{
    port_group_registers_t *p = (port_group_registers_t *)port;
    p->PORT_OUTSET = mask;
}

static inline void gpio_ll_clear(gpio_port_t port, uword_t mask)
{
    port_group_registers_t *p = (port_group_registers_t *)port;
    p->PORT_OUTCLR = mask;
}

static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask)
{
    port_group_registers_t *p = (port_group_registers_t *)port;
    p->PORT_OUTTGL = mask;
}

static inline void gpio_ll_write(gpio_port_t port, uword_t mask)
{
    port_group_registers_t *p = (port_group_registers_t *)port;
    p->PORT_OUT = mask;
}

static inline void gpio_ll_switch_dir_output(gpio_port_t port, uword_t outputs)
{
    port_group_registers_t *p = (port_group_registers_t *)port;
    p->PORT_DIRSET = outputs;
}

static inline void gpio_ll_switch_dir_input(gpio_port_t port, uword_t inputs)
{
    port_group_registers_t *p = (port_group_registers_t *)port;
    p->PORT_DIRCLR = inputs;
}

static inline gpio_port_t gpio_get_port(gpio_t pin)
{
    /* GPIO LL and legacy GPIO API may disagree on what is the GPIO base
     * address if one is using the IOBUS and the other is using the APB for
     * access. In this case, we need to do impedance matching by adding the
     * offset. */
    const uintptr_t gpio_ll_base = GPIO_PORT_0;
    const uintptr_t gpio_legacy_base = GPIO_PIN(0, 0) & ~(0x1f);
    uintptr_t addr = (pin & ~(0x1f));

    return addr + (gpio_ll_base - gpio_legacy_base);
}

static inline uint8_t gpio_get_pin_num(gpio_t pin)
{
    return pin & 0x1f;
}

static inline gpio_port_t gpio_port_pack_addr(void *addr)
{
    return (gpio_port_t)addr;
}

static inline void * gpio_port_unpack_addr(gpio_port_t port)
{
    if (port < GPIO_PORT(0)) {
        return (void *)port;
    }
    if (port > GPIO_PORT(ARRAY_SIZE(GPIO_IOBUS_BASE->GROUP))) {
        return (void *)port;
    }

    return NULL;
}

static inline bool is_gpio_port_num_valid(uint_fast8_t num)
{
    return (num < ARRAY_SIZE(GPIO_IOBUS_BASE->GROUP));
}

#endif /* DOXYGEN */
#ifdef __cplusplus
}
#endif

/** @} */

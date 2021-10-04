/*
 * Copyright (C) 2021 Mesotic SAS
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_polarfire
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "plic.h"

#include "periph/timer.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static timer_cb_t _callback[2];
static void* _arg[2];
uint8_t timer1_isr(void);
uint8_t timer2_isr(void);

static inline TIMER_TypeDef *dev(tim_t tim)
{
    return timer_config[tim].dev;
}

static inline uint32_t _timer_value(uint32_t val)
{
    return val * 150;
}

static bool _is_oneshot(tim_t tim)
{
    if (((dev(tim)->TIM1_CTRL & TIM1_MODE_MASK) >> TIM1_MODE_SHIFT) == MSS_TIMER_ONE_SHOT_MODE)
    {
        return true;
    }
    return false;
}

int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
{
    (void)freq;
    mss_config_clk_rst(MSS_PERIPH_TIMER, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);
    if (timer_config[tim].num == 1)
    {
        _callback[0] = cb;
        _arg[0] = arg;
        plic_set_isr_cb(TIMER1_PLIC, timer1_isr);
        plic_set_priority(TIMER1_PLIC, 2);
        puts("TIMER1_PLIC");
    }
    else
    {
        _callback[1] = cb;
        _arg[1] = arg;
        plic_set_isr_cb(TIMER2_PLIC, timer2_isr);
    }
    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    (void)channel;
    if (timer_config[tim].num == 1)
    {
        MSS_TIM1_init(dev(tim), MSS_TIMER_ONE_SHOT_MODE);
        MSS_TIM1_enable_irq(dev(tim));
        MSS_TIM1_load_immediate(dev(tim), _timer_value(value));
        MSS_TIM1_start(dev(tim));
    }
    else
    {
        MSS_TIM2_init(dev(tim), MSS_TIMER_ONE_SHOT_MODE);
        MSS_TIM2_enable_irq(dev(tim));
        MSS_TIM2_load_immediate(dev(tim), _timer_value(value));
    }
    return 0;
}

int timer_set_periodic(tim_t tim, int channel, unsigned int value, uint8_t flags)
{
    (void)channel;
    (void)flags;
    if (timer_config[tim].num == 1)
    {
        MSS_TIM1_init(dev(tim), MSS_TIMER_PERIODIC_MODE);
        MSS_TIM1_load_immediate(dev(tim), _timer_value(value));
        MSS_TIM1_start(dev(tim));
        MSS_TIM1_enable_irq(dev(tim));
        puts("SET PERIODIC");
    }
    else
    {
        MSS_TIM2_init(dev(tim), MSS_TIMER_PERIODIC_MODE);
        MSS_TIM2_enable_irq(dev(tim));
        MSS_TIM2_load_immediate(dev(tim), _timer_value(value));
    }
    return 0;
}

int timer_clear(tim_t tim, int channel)
{ 
    (void)channel;
    if (timer_config[tim].num == 1)
    {
        MSS_TIM1_disable_irq(dev(tim));
    }
    else
    {
        MSS_TIM2_disable_irq(dev(tim));
    }
    return 0;
}

unsigned int timer_read(tim_t tim)
{
    if (timer_config[tim].num == 1)
    {
        return MSS_TIM1_get_current_value(dev(tim));
    }
    else
    {
        return MSS_TIM2_get_current_value(dev(tim));
    }
}

void timer_start(tim_t tim)
{
    if (timer_config[tim].num == 1)
    {
        MSS_TIM1_start(dev(tim));
    }
    else
    {
        MSS_TIM2_start(dev(tim));
    }
}

void timer_stop(tim_t tim)
{
    if (timer_config[tim].num == 1)
    {
        MSS_TIM1_stop(dev(tim));
    }
    else
    {
        MSS_TIM2_stop(dev(tim));
    }
}

uint8_t timer1_isr(void)
{
    if (_is_oneshot(0))
    {
        MSS_TIM1_disable_irq(dev(0));
    }
    MSS_TIM1_clear_irq(dev(0));
    _callback[0](_arg[0], 0);
    return 0;
}

uint8_t timer2_isr(void)
{
    if (_is_oneshot(1))
    {
        MSS_TIM1_disable_irq(dev(1));
    }
    MSS_TIM1_clear_irq(dev(1));
    _callback[1](_arg[1], 1);
    return 0;
}
/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       CPU specific part of the timer API
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

static inline bool timer_poll_channel(tim_t tim, int channel)
{
    /**
     * ~~~~~~~~~~~~~~~{.c}
     *  typedef union
     *  {
     *      tc_count8_registers_t          COUNT8;
     *      tc_count16_registers_t         COUNT16;
     *      tc_count32_registers_t         COUNT32;
     *  } tc_registers_t;
     * ~~~~~~~~~~~~~~~
     * TC_INTFLAG has for all timerwidth the same offset, so it does not matter which union member is used.
     */
    switch (channel) {
    case 0:
        return timer_config[tim].dev->COUNT32.TC_INTFLAG & TC_INTFLAG_MC0_Msk;
    case 1:
        return timer_config[tim].dev->COUNT32.TC_INTFLAG & TC_INTFLAG_MC1_Msk;
    default:
        return false;
    }
}

#endif /* DOXYGEN */
#ifdef __cplusplus
}
#endif

/** @} */

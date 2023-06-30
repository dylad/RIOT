/*
 * Copyright (C) 2017 HAW Hamburg
 *               2017 Freie Universität Berlin
 *               2023 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"

/**
 * @name    Default ADC reference, gain configuration and acquisition time
 *
 * Can be overridden by the board configuration if needed. The default
 * configuration uses the full VDD (typically 3V3) as reference and samples for
 * 10us.
 * @{
 */
#ifndef ADC_REF
#define ADC_REF             SAADC_CH_CONFIG_REFSEL_VDD1_4
#endif
#ifndef ADC_GAIN
#define ADC_GAIN            SAADC_CH_CONFIG_GAIN_Gain1_4
#endif
#ifndef ADC_TACQ
#define ADC_TACQ            SAADC_CH_CONFIG_TACQ_10us
#endif
/** @} */

/* Compatibility wrapper defines for nRF9160 and nRF5340 */
#ifdef NRF_SAADC_S
#define NRF_SAADC NRF_SAADC_S
#endif

#define ADC_MODE(x) adc_channels[x].mode
#define ADC_POL(x)  adc_channels[x].pol

/**
 * @brief   Lock to prevent concurrency issues when used from different threads
 */
static mutex_t lock = MUTEX_INIT;

/**
 * @brief   We use a static result buffer so we do not have to reprogram the
 *          result pointer register
 */
static int16_t result;

static inline void prep(void)
{
    mutex_lock(&lock);
    NRF_SAADC->ENABLE = 1;
}

static inline void done(void)
{
    NRF_SAADC->ENABLE = 0;
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    if (line >= ADC_NUMOF) {
        return -1;
    }

    prep();

    /* prevent multiple initialization by checking the result ptr register */
    if (NRF_SAADC->RESULT.PTR != (uint32_t)&result) {
        /* set data pointer and the single channel we want to convert */
        NRF_SAADC->RESULT.MAXCNT = 1;
        NRF_SAADC->RESULT.PTR = (uint32_t)&result;

        /* configure the channel:
         * - bypass resistor ladder+
         * - acquisition time as defined by board (or 10us as default)
         * - reference and gain as defined by board (or VDD as default)
         * - no oversampling */
        NRF_SAADC->CH[line].CONFIG = ((ADC_GAIN << SAADC_CH_CONFIG_GAIN_Pos) |
                                     (ADC_REF << SAADC_CH_CONFIG_REFSEL_Pos) |
                                     (ADC_MODE(line) << SAADC_CH_CONFIG_MODE_Pos) |
                                     (ADC_TACQ << SAADC_CH_CONFIG_TACQ_Pos));
        if (ADC_MODE(line) == ADC_SINGLE_ENDED) {
            NRF_SAADC->CH[line].PSELP = adc_channels[line].psel;
            NRF_SAADC->CH[line].PSELN = SAADC_CH_PSELN_PSELN_NC;
        } else {
            if (ADC_POL(line) == ADC_PSEL_POS) {
                NRF_SAADC->CH[line].PSELP = adc_channels[line].psel;
                NRF_SAADC->CH[line].PSELN = SAADC_CH_PSELN_PSELN_NC;
            } else if (ADC_POL(line) == ADC_PSEL_NEG) {
                NRF_SAADC->CH[line].PSELP = SAADC_CH_PSELP_PSELP_NC;
                NRF_SAADC->CH[line].PSELN = adc_channels[line].psel;
            } else {
                /* Board misconfiguration detected */
                assert(0);
            }
        }
        NRF_SAADC->CH[line].PSELP = adc_channels[line].psel;
        /* Make sure negative channel is NC in single-ended mode */
        if (ADC_MODE(line) == ADC_SINGLE_ENDED) {
            NRF_SAADC->CH[line].PSELN = SAADC_CH_PSELN_PSELN_NC;
        } else
        /* Disable oversampling */
        NRF_SAADC->OVERSAMPLE = SAADC_OVERSAMPLE_OVERSAMPLE_Bypass;

        /* calibrate SAADC */
        NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
        NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;
        while (NRF_SAADC->EVENTS_CALIBRATEDONE == 0) {}
    }

    done();

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    assert(line < ADC_NUMOF);

    /* check if resolution is valid */
    if (res > 2) {
        return -1;
    }

    /* prepare device */
    prep();
   
    /* set resolution */
    NRF_SAADC->RESOLUTION = res;

    /* start the SAADC and wait for the started event */
    NRF_SAADC->EVENTS_STARTED = 0;
    NRF_SAADC->TASKS_START = 1;
    while (NRF_SAADC->EVENTS_STARTED == 0) {}

    /* trigger the actual conversion */
    NRF_SAADC->EVENTS_END = 0;
    NRF_SAADC->TASKS_SAMPLE = 1;
    while (NRF_SAADC->EVENTS_END == 0) {}

    /* stop the SAADC */
    NRF_SAADC->EVENTS_STOPPED = 0;
    NRF_SAADC->TASKS_STOP = 1;
    while (NRF_SAADC->EVENTS_STOPPED == 0) {}

    /* free device */
    done();

    /* hack -> the result can be a small negative number when a AINx pin is
     * connected via jumper wire a the board's GND pin. There seems to be a
     * slight difference between the internal CPU GND and the board's GND
     * voltage levels?! (observed on nrf52dk and nrf52840dk) */
    return (result < 0) ? 0 : (int)result;
}

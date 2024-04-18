/*
 * Copyright (C) 2015 INRIA
 * Copyright (C) 2015 Eistec AB
 * Copyright (C) 2016 OTA keys
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Crash handling functions implementation for ARM Cortex-based MCUs
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Toon Stegen <toon.stegen@altran.com>
 */

#include "cpu.h"
#include "log.h"

void panic_arch(void)
{
#ifdef DEVELHELP
    uint32_t ipsr = __get_IPSR() & IPSR_ISR_Msk;
    if(ipsr) {
        int32_t isr = (((int)ipsr) - 16);
        LOG_ERROR("Inside isr %ld\n", isr);
        /* Print associated status register if any */
        LOG_ERROR("SCB->CFSR:0x%08lx\n", SCB->CFSR);
        if (isr == MemoryManagement_IRQn) {
            LOG_ERROR("SCB->MMFAR:0x%08lx\n", SCB->MMFAR);
        }
        else if (isr == BusFault_IRQn) {
            LOG_ERROR("SCB->BFAR:0x%08lx\n", SCB->BFAR);
        }
    }

    /* The bkpt instruction will signal to the debugger to break here. */
    __asm__("bkpt #0");
#endif
}

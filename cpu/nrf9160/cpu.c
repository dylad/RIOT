/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf9160
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"
#include "kernel_init.h"
#include "nrf_clock.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

/**
 * @brief    LFCLK Clock selection configuration guard
*/
#if ((CLOCK_LFCLK != CLOCK_LFCLKSRC_SRC_LFRC) && \
     (CLOCK_LFCLK != CLOCK_LFCLKSRC_SRC_LFXO))
#error "LFCLK init: CLOCK_LFCLK has invalid value"
#endif

/**
 * @brief   Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize hf clock */
    clock_init_hf();

    *((volatile uint32_t *)0x5003AEE4) = 0xE;

#ifdef NVMC_ICACHECNF_CACHEEN_Msk
    /* enable instruction cache */
    NRF_NVMC_S->ICACHECNF = (NVMC_ICACHECNF_CACHEEN_Msk);
#endif

    /* call cortexm default initialization */
    cortexm_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();

    if (IS_USED(MODULE_NRFXLIB_NRF_MODEM)) {
        *((volatile uint32_t *)0x5000470Cul) = 0x0;
        *((volatile uint32_t *)0x50004710ul) = 0x1;
        clock_start_lf();
        *((volatile uint32_t *)0x50004A38) = 0x01ul;
        NRF_REGULATORS_S->DCDCEN = 1;
        /* Move POWER and IPC peripherals and half of the RAM to unsecure domain
           as required by NRF modem library */
        NRF_SPU_S->PERIPHID[IPC_IRQn].PERM &= ~SPU_EXTDOMAIN_PERM_SECATTR_Msk;
        NRF_SPU_S->PERIPHID[CLOCK_POWER_IRQn].PERM &= ~SPU_EXTDOMAIN_PERM_SECATTR_Msk;
        NRF_SPU_S->PERIPHID[4].PERM &= ~SPU_EXTDOMAIN_PERM_SECATTR_Msk;

        /* Mark first half of RAM as non secure */
        for (unsigned i=0; i<15; i++)
        {
            NRF_SPU_S->RAMREGION[i].PERM &= ~SPU_RAMREGION_PERM_SECATTR_Msk;
        }

        SCB->NSACR |= (3UL << 10UL);
        SCB->CPACR |= (3UL << 20ul) | (3UL << 22ul);
        __DSB();
        __ISB();

    }
}

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

#if !defined(NRF_FICR_NS)
    #define NRF_FICR_NS_BASE 0x2003E000
    #define NRF_FICR_NS ((NRF_FICR_Type*)          NRF_FICR_NS_BASE)
#endif

/* Workaround to allow NS code to access FICR. Override NRF_FICR_NS to move FICR_NS buffer. */
#define FICR_SIZE 0x1000ul
#define RAM_BASE 0x20000000ul
#define RAM_END  0x2FFFFFFFul
#define SPU_RAMREGION_SIZE 0x2000ul

/* Copy FICR_S to FICR_NS RAM region */
void SystemStoreFICRNS(void)
{
    if ((uint32_t)NRF_FICR_NS < RAM_BASE || (uint32_t)NRF_FICR_NS + FICR_SIZE > RAM_END)
    {
        /* FICR_NS is not in RAM. */
        return;
    }
    /* Copy FICR to NS-accessible RAM block. */
    volatile uint32_t * from            = (volatile uint32_t *)((uint32_t)NRF_FICR_S + (FICR_SIZE - sizeof(uint32_t)));
    volatile uint32_t * to              = (volatile uint32_t *)((uint32_t)NRF_FICR_NS + (FICR_SIZE - sizeof(uint32_t)));
    volatile uint32_t * copy_from_end   = (volatile uint32_t *)NRF_FICR_S;
    while (from >= copy_from_end)
    {
        *(to--) = *(from--);
    }

    /* Make RAM region NS. */
    uint32_t ram_region = ((uint32_t)NRF_FICR_NS - (uint32_t)RAM_BASE) / SPU_RAMREGION_SIZE;
    __DSB();
    NRF_SPU_S->RAMREGION[ram_region].PERM &= ~(1 << SPU_RAMREGION_PERM_SECATTR_Pos);
}

bool uicr_HFXOCNT_erased(void)
{
    if (NRF_UICR_S->HFXOCNT == 0xFFFFFFFFul) {
        return true;
    }
    return false;
}
    
    
bool uicr_HFXOSRC_erased(void)
{
    if ((NRF_UICR_S->HFXOSRC & UICR_HFXOSRC_HFXOSRC_Msk) != UICR_HFXOSRC_HFXOSRC_TCXO) {
        return true;
    }
    return false;
}

void errata_uicr_hfxo(void)
{
    if (uicr_HFXOSRC_erased() || uicr_HFXOCNT_erased()) {
        /* Wait for pending NVMC operations to finish */
        while (NRF_NVMC_S->READY != NVMC_READY_READY_Ready);
          
        /* Enable write mode in NVMC */
        NRF_NVMC_S->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (NRF_NVMC_S->READY != NVMC_READY_READY_Ready);
          
        if (uicr_HFXOSRC_erased()){
            /* Write default value to UICR->HFXOSRC */
            NRF_UICR_S->HFXOSRC = (NRF_UICR_S->HFXOSRC & ~UICR_HFXOSRC_HFXOSRC_Msk) | UICR_HFXOSRC_HFXOSRC_TCXO;
            while (NRF_NVMC_S->READY != NVMC_READY_READY_Ready);
            }
          
        if (uicr_HFXOCNT_erased()){
            /* Write default value to UICR->HFXOCNT */
            NRF_UICR_S->HFXOCNT = (NRF_UICR_S->HFXOCNT & ~UICR_HFXOCNT_HFXOCNT_Msk) | 0x20;
            while (NRF_NVMC_S->READY != NVMC_READY_READY_Ready);
        }
                
        /* Enable read mode in NVMC */
        NRF_NVMC_S->CONFIG = NVMC_CONFIG_WEN_Ren;
        while (NRF_NVMC_S->READY != NVMC_READY_READY_Ready);
          
        /* Reset to apply clock select update */
        NVIC_SystemReset();
    }
}
/**
 * @brief   Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    NRF_POWER_S->EVENTS_SLEEPENTER = (POWER_EVENTS_SLEEPENTER_EVENTS_SLEEPENTER_NotGenerated << POWER_EVENTS_SLEEPENTER_EVENTS_SLEEPENTER_Pos);
    NRF_POWER_S->EVENTS_SLEEPEXIT = (POWER_EVENTS_SLEEPEXIT_EVENTS_SLEEPEXIT_NotGenerated << POWER_EVENTS_SLEEPEXIT_EVENTS_SLEEPEXIT_Pos);

    /* initialize hf clock */
    *((volatile uint32_t *)0x5000470Cul) = 0x0;
    *((volatile uint32_t *)0x50004710ul) = 0x1;
    *((volatile uint32_t *)0x50004A38) = 0x01ul;
    NRF_REGULATORS_S->DCDCEN = REGULATORS_DCDCEN_DCDCEN_Enabled << REGULATORS_DCDCEN_DCDCEN_Pos;

    *((volatile uint32_t *)0x5003AEE4) = 0xE;
    SystemStoreFICRNS();
    errata_uicr_hfxo();
    clock_init_hf();
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

    /* Initialization if NRF modem is used */
    if (IS_USED(MODULE_NRFXLIB_NRF_MODEM)) {
        /* Use External HFXO Crystal */
        clock_hfxo_request();
        /* Start LFCLK as needed by the modem */
        clock_start_lf();
        /* Move POWER and IPC peripherals and half of the RAM to unsecure domain
           as required by NRF modem library */   
        NRF_SPU_S->PERIPHID[IPC_IRQn].PERM &= ~SPU_EXTDOMAIN_PERM_SECATTR_Msk;
        NRF_SPU_S->PERIPHID[CLOCK_POWER_IRQn].PERM &= ~SPU_EXTDOMAIN_PERM_SECATTR_Msk;
        /* Mark first half of RAM as non secure as requested by Nordic
           modem documentation*/
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

/*
 * Copyright (C) 2019 Mesotic SAS <dylan.laduranty@mesotic.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <string.h>

#include "rpmsg_platform.h"
#include "rpmsg_env.h"
#ifdef 0
#include "clk_conf.h"
#endif
//#include "fsl_device_registers.h"
//#include "fsl_mailbox.h"

/* RIOT header */
#include "xtimer.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
#include "mcmgr.h"
#endif

#if defined(RL_USE_ENVIRONMENT_CONTEXT) && (RL_USE_ENVIRONMENT_CONTEXT == 1)
#error "This RPMsg-Lite port requires RL_USE_ENVIRONMENT_CONTEXT set to 0"
#endif

static int isr_counter = 0;
static int disable_counter = 0;
static void *platform_lock;

#define APP_MU_IRQ_PRIORITY (1U)

#ifndef MU_CR_NMI_MASK
#define MU_CR_NMI_MASK 0U
#endif

#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
static void mcmgr_event_handler(uint16_t vring_idx, void *context)
{
    env_isr(vring_idx);
}
#else
void MAILBOX_IRQHandler(void)
{
    DEBUG("%s",__FUNCTION__);
}
#endif

void platform_global_isr_disable(void)
{
    __asm volatile("cpsid i");
}


void platform_global_isr_enable(void)
{
    __asm volatile("cpsie i");
}

static inline void MU_EnableInterrupts(MU_Type *base, uint32_t mask)
{
    uint32_t reg = base->CR;
    reg = (reg & ~(MU_CR_GIRn_MASK | MU_CR_NMI_MASK)) | mask;
    base->CR = reg;
}

int platform_init_interrupt(unsigned int vector_id, void *isr_data)
{
    /* Register ISR to environment layer */
    env_register_isr(vector_id, isr_data);
    /* Prepare the MU Hardware, enable channel 1 interrupt */
    env_lock_mutex(platform_lock);
    assert(0 <= isr_counter);
    if (!isr_counter) {
        MU_EnableInterrupts(MUB, (1U << 27U) >> RPMSG_MU_CHANNEL);
    }
    isr_counter++;
    env_unlock_mutex(platform_lock);
    return 0;
}

int platform_deinit_interrupt(unsigned int vector_id)
{
    (void) vector_id;
    DEBUG("%s",__FUNCTION__);
    return 0;
}

void MU_SendMsg(MU_Type *base, uint32_t regIndex, uint32_t msg)
{
    assert(regIndex < MU_TR_COUNT);

    /* Wait TX register to be empty. */
    while (!(base->SR & ((1U << (MU_SR_TEn_SHIFT + 3U)) >> regIndex)))
    {
    }

    base->TR[regIndex] = msg;
}

void platform_notify(unsigned int vector_id)
{
    /* As Linux suggests, use MU->Data Channel 1 as communication channel */
    uint32_t msg = (uint32_t)(vector_id << 16);
    env_lock_mutex(platform_lock);
    MU_SendMsg(MUB, RPMSG_MU_CHANNEL, msg);
    env_unlock_mutex(platform_lock);
}

static inline uint32_t MU_GetStatusFlags(MU_Type *base)
{
    return (base->SR & (MU_SR_TEn_MASK | MU_SR_RFn_MASK | MU_SR_GIPn_MASK | MU_SR_EP_MASK | MU_SR_FUP_MASK
#if (defined(FSL_FEATURE_MU_HAS_SR_RS) && FSL_FEATURE_MU_HAS_SR_RS)
                        | MU_SR_RS_MASK
#endif
#if (defined(FSL_FEATURE_MU_HAS_RESET_INT) && FSL_FEATURE_MU_HAS_RESET_INT)
                        | MU_SR_RDIP_MASK | MU_SR_RAIP_MASK
#endif
#if (defined(FSL_FEATURE_MU_HAS_SR_MURIP) && FSL_FEATURE_MU_HAS_SR_MURIP)
                        | MU_SR_MURIP_MASK
#endif
#if (defined(FSL_FEATURE_MU_HAS_SR_HRIP) && FSL_FEATURE_MU_HAS_SR_HRIP)
                        | MU_SR_HRIP_MASK
#endif
                        ));
}
static inline uint32_t MU_ReceiveMsgNonBlocking(MU_Type *base, uint32_t regIndex)
{
    assert(regIndex < MU_TR_COUNT);

    return base->RR[regIndex];
}
int isr_mu_m4(void);
int isr_mu_m4(void)
{
    uint32_t channel;

    if (((1U << 27U) >> RPMSG_MU_CHANNEL) & MU_GetStatusFlags(MUB))
    {
        channel = MU_ReceiveMsgNonBlocking(MUB, RPMSG_MU_CHANNEL);
        env_isr(channel >> 16);
    }

    return 0;
}

/**
 * platform_time_delay
 *
 * @param num_msec Delay time in ms.
 *
 * This is not an accurate delay, it ensures at least num_msec passed when return.
 */
void platform_time_delay(int num_msec)
{
    uint32_t loop;
    /* Calculate the CPU loops to delay, each loop has 3 cycles */
    loop = CLOCK_CORECLOCK / 3 / 1000 * num_msec;

    /* There's some difference among toolchains, 3 or 4 cycles each loop */
    while (loop)
    {
        __NOP();
        loop--;
    }
}


int platform_in_isr(void)
{
    return ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0);
}

int platform_interrupt_enable(unsigned int vector_id)
{
    assert(0 < disable_counter);

    __asm volatile("cpsid i");
    disable_counter--;

    if (!disable_counter)
        NVIC_EnableIRQ(MU_M4_IRQn);
    __asm volatile("cpsie i");
    return (vector_id);
}

int platform_interrupt_disable(unsigned int vector_id)
{
    assert(0 <= disable_counter);

    __asm volatile("cpsid i");
    // virtqueues use the same NVIC vector
    // if counter is set - the interrupts are disabled
    if (!disable_counter)
        NVIC_DisableIRQ(MU_M4_IRQn);

    disable_counter++;
    __asm volatile("cpsie i");
    return (vector_id);
}

/**
 * platform_map_mem_region
 *
 * Dummy implementation
 *
 */
void platform_map_mem_region(unsigned int vrt_addr, unsigned int phy_addr, unsigned int size, unsigned int flags)
{
    (void) vrt_addr;
    (void) phy_addr;
    (void) size;
    (void) flags;
    DEBUG("%s",__FUNCTION__);
}

/**
 * platform_cache_all_flush_invalidate
 *
 * Dummy implementation
 *
 */
void platform_cache_all_flush_invalidate(void)
{
    DEBUG("%s",__FUNCTION__);
}

/**
 * platform_cache_disable
 *
 * Dummy implementation
 *
 */
void platform_cache_disable(void)
{
    DEBUG("%s",__FUNCTION__);
}

/**
 * platform_vatopa
 *
 * Dummy implementation
 *
 */
unsigned long platform_vatopa(void *addr)
{
    return ((unsigned long)addr);
}

/**
 * platform_patova
 *
 * Dummy implementation
 *
 */
void *platform_patova(unsigned long addr)
{
    return ((void *)addr);
}

/**
 * platform_init
 *
 * platform/environment init
 */
int platform_init(void)
{
    //ccm_set_root_clk(CCM_ROOT_CLK_RDC, 1, 1, 1);
    ccm_set_ccgr(CCM_CCGR_MU, CLK_ENABLE_ALL_DOMAINS);
    ccm_en_root_clk(CCM_ROOT_CLK_RDC, 1);
    NVIC_SetPriority(MU_M4_IRQn, APP_MU_IRQ_PRIORITY);
    NVIC_EnableIRQ(MU_M4_IRQn);
    /* Create lock used in multi-instanced RPMsg */
    env_create_mutex(&platform_lock, 1);
    return 0;
}

int platform_deinit(void)
{
    DEBUG("%s",__FUNCTION__);
    return 0;
}
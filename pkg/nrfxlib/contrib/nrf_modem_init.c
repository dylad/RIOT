/*
 * Copyright (C) 2021-2022 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Dylan Laduranty <dylan.laduranty@mesotic.com
 */

#include "kernel_defines.h"
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "tlsf.h"
#include "ztimer.h"

#include "nrf_modem_os.h"
#include "nrf_modem_params.h"
#include "nrf_modem.h"
#include "nrf_modem_at.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define DEFAULT_SHMEM_SIZE 0x800

tlsf_t tlsf;

/* Declare static shared memory buffer for communication between
   nRF9160 MCU and its modem, enforce alignment */
static uint8_t tx_shbuf[DEFAULT_SHMEM_SIZE]    __attribute__((aligned(4)));
static uint8_t rx_shbuf[DEFAULT_SHMEM_SIZE]    __attribute__((aligned(4)));
static uint8_t ctrl_shbuf[NRF_MODEM_SHMEM_CTRL_SIZE]  __attribute__((aligned(4)));
static uint8_t trace_shbuf[DEFAULT_SHMEM_SIZE] __attribute__((aligned(4)));

void fault_handler(struct nrf_modem_fault_info *fault_info);
void dfu_handler(uint32_t dfu_result);

static const struct nrf_modem_init_params nrf_modem_default_init_params __attribute__((aligned(4))) = {
    .shmem = {
        .ctrl = {
            .base = (uint32_t)ctrl_shbuf,
            .size = NRF_MODEM_SHMEM_CTRL_SIZE,
        },
        .tx = {
            .base = (uint32_t)tx_shbuf,
            .size = DEFAULT_SHMEM_SIZE,
        },
        .rx = {
            .base = (uint32_t)rx_shbuf,
            .size = DEFAULT_SHMEM_SIZE,
        },
        .trace = {
            .base = (uint32_t)trace_shbuf,
            .size = DEFAULT_SHMEM_SIZE,
        },
    },
    .ipc_irq_prio = 8,
    .fault_handler = fault_handler,
    .dfu_handler = dfu_handler,
};

void fault_handler(struct nrf_modem_fault_info *fault_info){
    (void)fault_info;
    DEBUG_PUTS("FAULT_HANDLER");
};

void dfu_handler(uint32_t dfu_result) {
    (void)dfu_result;
    DEBUG_PUTS("DFU_HANDLER");
}

void nrf_modem_bootstrap(void)
{
    int ret;

    /* Create a pool with TX buffer as nRF modem library need to malloc
       some data from it */
    //tlsf = tlsf_create_with_pool(tx_shbuf, DEFAULT_SHMEM_SIZE);
    DEBUG("SHCTRL:%p\n", ctrl_shbuf);
    DEBUG("SHTX:%p\n", tx_shbuf);
    DEBUG("SHRX:%p\n", rx_shbuf);
    memset(ctrl_shbuf, 0, NRF_MODEM_SHMEM_CTRL_SIZE);
    memset(rx_shbuf, 0, DEFAULT_SHMEM_SIZE);
    memset(tx_shbuf, 0, DEFAULT_SHMEM_SIZE);
    memset(trace_shbuf, 0, DEFAULT_SHMEM_SIZE);
    /* Initialize Modem librairy */
    DEBUG_PUTS("___________");
    DEBUG("UICR->HFXOCNT:%lx\n", NRF_UICR_S->HFXOCNT);
    DEBUG("UICR->HFXOSRC:%lx\n", NRF_UICR_S->HFXOSRC);
    DEBUG("CLOCK->HFCLKSTAT:%lx\n", NRF_CLOCK_NS->HFCLKSTAT);
    DEBUG("CLOCK->LFCLKSTAT:%lx\n", NRF_CLOCK_NS->LFCLKSTAT);
    DEBUG("POWER->POWERSTATUS:%lx\n", NRF_POWER_NS->POWERSTATUS);

    if(nrf_modem_is_initialized()) {
        nrf_modem_shutdown();
         DEBUG("Shutting down...\n");
    }

    ret = nrf_modem_init(&nrf_modem_default_init_params);
    if (ret != 0) {
        DEBUG("nRF modem library init failed with error:%d\n", -ret);
        return;
    }

    DEBUG_PUTS("nRF modem libray initialized");

    /* Setup modem with board default parameters */
#if 0
    if (nrf_modem_setup() != 0) {
        DEBUG_PUTS("nrf_modem_setup failed, check your board parameters");
        /* Setup failed, shutdown modem */
        nrf_modem_shutdown();
        return;
    }
#endif
}

/** @} */

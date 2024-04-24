/*
 * Copyright (C) 2021-2024 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nrf_modem
 * @{
 *
 * @file
 * @brief       RIOT specific implementation of nrf_modem_os_* functions
 *              used by nrf modem
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "nrf_modem_os.h"
#include "nrf_modem_trace.h"
#include "nrf_errno.h"

#include "ztimer.h"
#include "tlsf.h"
#include "sema.h"
#include "mutex.h"
#include "busy_wait.h"

#include "cpu.h"
#include "nrf_clock.h"
#include "periph_cpu.h"
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>

#include "nrfx_ipc.h"

/* Enabling debug may break timing, use it with care */
#define ENABLE_DEBUG 1
#include "debug.h"

extern void *__real_malloc(size_t size);
extern void __real_free(void *ptr);
extern void *__real_realloc(void *ptr, size_t size);
struct nrf_modem_trace_data* read_frags;

ztimer_t wait_timer;
bool time_exp;
ztimer_now_t started;
extern tlsf_t tlsf;
static mutex_t _lock;
static bool down = true;
static mutex_t mutexes[1] __attribute__((aligned(4)));;
static sema_t semas[NRF_MODEM_OS_NUM_SEM_REQUIRED] __attribute__((aligned(4)));;
static unsigned mutex_idx = 0;
static unsigned sema_idx = 0;

static void _timeout(void* arg)
{
    (void)arg;
    unsigned irq = irq_disable();
    time_exp = true;
    irq_restore(irq);
}

void nrf_modem_os_init(void) {
    DEBUG("\n[nrf_modem_os]:%s\n", __FUNCTION__);
    wait_timer.callback = _timeout;
    wait_timer.arg = 0;
    down = false;
}

void nrf_modem_os_shutdown(void)
{
    /* When shutdown is called, all pending calls to
    nrf_modem_os_timedwait exit and return -NRF_ESHUTDOWN.  */
    down = true;
    DEBUG("%s TODO\n", __func__);
}

void nrf_modem_os_event_notify(uint32_t context)
{
    (void)context;
    //DEBUG_PUTS("OSNOTIF\n");
    LED3_ON;
}

int nrf_modem_os_sleep(uint32_t timeout)
{
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    ztimer_sleep(ZTIMER_MSEC, timeout);
    return 0;
}

int nrf_modem_os_sem_init(void **sem, unsigned int initial_count,
                          unsigned int limit)
{
    (void)limit;

    DEBUG("\n[nrf_modem_os]:%s:%p(%d, %d) currently:%d out of %d\n", __FUNCTION__,*sem, initial_count, limit,
          sema_idx, NRF_MODEM_OS_NUM_SEM_REQUIRED);

    if (sema_idx >= NRF_MODEM_OS_NUM_SEM_REQUIRED) {
        DEBUG("no more sema available, increase NRF_MODEM_OS_NUM_SEM_REQUIRED\n");
        return -ENOMEM;
    }
    /* Check if mutex is already used, re-init if this is the case */
    if (*sem) {
        sema_create((sema_t*)*sem, initial_count);
        return 0;
    }

    sema_create(&(semas[sema_idx]), initial_count);
    *sem = &(semas[sema_idx]);
    DEBUG("Sema created at %p\n", *sem);
    sema_idx++;
    return 0;
}
//give ++
//post ++
void nrf_modem_os_sem_give(void *sem)
{
    int ret;
    sema_t* sema = (sema_t*)sem;
    DEBUG("\n[nrf_modem_os]:%s %p\n", __FUNCTION__, sem);
    ret = sema_post(sema);
    if ( ret != 0) {
       assert(0);
    }
}
//take --
//wait --
int nrf_modem_os_sem_take(void *sem, int timeout)
{
    sema_t* sema = (sema_t*)sem;
    int ret = 0;
    
    DEBUG("\n[nrf_modem_os]:%s %p, %d\n", __FUNCTION__, sem, timeout);

    if (timeout == NRF_MODEM_OS_NO_WAIT)
    {
        ret = sema_wait_timed_ztimer(sema, ZTIMER_MSEC, 0);
    }
    else if (timeout == NRF_MODEM_OS_FOREVER) {
        /* Wait for semaphore to be released */
        do {
            ret = sema_wait_timed_ztimer(sema, ZTIMER_MSEC, UINT32_MAX);
        } while (ret != 0);
    }
    else {
        ret = sema_wait_timed_ztimer(sema, ZTIMER_MSEC, timeout);
    }

    if (ret != 0) {
        DEBUG_PUTS("SEMA TIMEOUT");
        return -NRF_EAGAIN;
    } else {
        return 0;
    }
}

unsigned int nrf_modem_os_sem_count_get(void *sem)
{
    sema_t* sema = (sema_t*)sem;
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    return sema_get_value(sema);
}

void *nrf_modem_os_shm_tx_alloc(size_t bytes) {
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    return tlsf_malloc(tlsf, bytes);
}

void nrf_modem_os_shm_tx_free(void *mem) {
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    tlsf_free(tlsf, mem);
}

/* malloc can be done in ISR context with Nordic Modem,
   but RIOT's malloc implementation is thread safe and
   prevent it from being call, thus, avoid it if we are
   from ISR context for compatibility */
void *nrf_modem_os_alloc(size_t bytes)
{
    void* ptr;
    DEBUG("[nrf_modem_os]:%s:%d bytes\n", __FUNCTION__, bytes);
    if (irq_is_in()) {
        if (mutex_trylock(&_lock)) {
            ptr = __real_malloc(bytes);
            mutex_unlock(&_lock);
        }
        else {
            return NULL;
        }
    }
    else {
        ptr = malloc(bytes);
    }

    return ptr;
}

void nrf_modem_os_free(void *mem)
{
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    if (irq_is_in()) {
        if (mutex_trylock(&_lock)) {
            __real_free(mem);
            mutex_unlock(&_lock);
        }
        else {
            assert(0);
        }
    }
    else {
        free(mem);
    }
}

void nrf_modem_os_busywait(int32_t usec) {
    /* Cannot use timer based as this function can be
       call from ISR context */
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    busy_wait(usec);
}

int32_t nrf_modem_os_timedwait(uint32_t context, int32_t* timeout)
{
    (void)context;

    DEBUG("[nrf_modem_os]:%s:%ld -> %ld\n", __FUNCTION__, context, *timeout);

    if (down) {
        return -NRF_ESHUTDOWN;
    }

    if (*timeout == NRF_MODEM_OS_FOREVER) {
        ztimer_sleep(ZTIMER_MSEC, UINT32_MAX);
    }
    unsigned irq = irq_disable();
    if (!ztimer_is_set(ZTIMER_MSEC, &wait_timer)) {
        ztimer_set(ZTIMER_MSEC, &wait_timer, *timeout);
        started = ztimer_now(ZTIMER_MSEC);
        time_exp = false;
    }

    if (time_exp == true) {
        *timeout = 0;
        irq_restore(irq);
        DEBUG("EGAIN\n");
        return -NRF_EAGAIN;
    }
    else {
        *timeout = *timeout - (ztimer_now(ZTIMER_MSEC) - started);
    }
    irq_restore(irq);

    return 0;
}

void nrf_modem_os_errno_set(int errno_val)
{
    (void)errno_val;
    DEBUG("[nrf_modem_os]:%s:%d\n", __FUNCTION__, errno_val);
    /* TODO: implement this */
}

bool nrf_modem_os_is_in_isr(void) {
    return irq_is_in();
}

int nrf_modem_os_mutex_init(void **mutex)
{
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    if (mutex_idx >= 1) {
        DEBUG("no more mutex available, increase NRF_MODEM_OS_NUM_MUTEX_REQUIRED\n");
        return -ENOMEM;
    }
    /* Check if mutex is already used, re-init if this is the case */
    if (*mutex) {
        mutex_init((mutex_t*)*mutex);
        return 0;
    }
    mutex_init(&mutexes[mutex_idx]);
    *mutex = &mutexes[mutex_idx];
    mutex_idx++;
    return 0;
}

int nrf_modem_os_mutex_unlock(void *mutex)
{
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    mutex_t* mut = (mutex_t*)mutex;
    mutex_unlock(mut);
    return 0;
}

int nrf_modem_os_mutex_lock(void *mutex, int timeout)
{
    mutex_t* mut = (mutex_t*) mutex;
    int ret = 0;
    DEBUG("[nrf_modem_os]:%s\n", __FUNCTION__);
    /* Try to lock a reentrant mutex with the given timeout. */
    if (timeout == NRF_MODEM_OS_NO_WAIT) {
        ret = mutex_trylock(mut);
    }
    else if (timeout == NRF_MODEM_OS_FOREVER) {
        mutex_lock(mut);
    }
    else {
        DEBUG("TODO: mutex w/ timeout");
    }

    if (ret != 0) {
        return -NRF_EAGAIN;
    }

    return 0;
}

void nrf_modem_os_log(int level, const char *str, ...)
{
  (void)level;
  (void)str;
}

void nrf_modem_os_logdump(int level, const char *str, const void *data, size_t len)
{
    (void)level;
    (void)str;
    (void)data;
    (void)len;
    DEBUG("[nrf_modem_os]:%s\n", __func__);
}

void isr_ipc(void){
    /* Forward IRQ to Nordic HAL */
    nrfx_ipc_irq_handler();
    cortexm_isr_end();
}

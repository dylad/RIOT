/*
 * Copyright (C) 2019 Mesotic SAS <dylan.laduranty@mesotic.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     pkg
 * @file
 * @brief       Implementation of RIOT-OS platform abstraction
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include "rpmsg_env.h"
#include "rpmsg_platform.h"
#include "virtqueue.h"
#include "rpmsg_compiler.h"

/* RIOT specific headers */
#include "sema.h"
#include "xtimer.h"
#include "msg.h"
#include "thread.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#include <stdlib.h>
#include <string.h>

static int env_init_counter = 0;
extern kernel_pid_t mpid;

/* Max supported ISR counts */
#define ISR_COUNT (32)
/*!
 * Structure to keep track of registered ISR's.
 */
struct isr_info
{
    void *data;
};

static struct isr_info isr_table[ISR_COUNT];

static int32_t env_in_isr(void)
{
     return platform_in_isr();
}

int32_t env_init(void)
{
    // verify 'env_init_counter'
    RL_ASSERT(env_init_counter >= 0);
    if (env_init_counter < 0)
    {
        return -1;
    }
    env_init_counter++;
    // multiple call of 'env_init' - return ok
    if (1 < env_init_counter)
    {
        return 0;
    }
    // first call
    memset(isr_table, 0, sizeof(isr_table));
    return platform_init();
}

int32_t env_deinit(void)
{
    return 0;
}

void *env_allocate_memory(uint32_t size)
{
    return (malloc(size));
}

void env_free_memory(void *ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}

void env_memset(void *ptr, int32_t value, uint32_t size)
{
    memset(ptr, value, size);
}

void env_memcpy(void *dst, void const *src, uint32_t len)
{
    memcpy(dst, src, len);
}

int32_t env_strcmp(const char *dst, const char *src)
{
    return (strcmp(dst, src));
}

void env_strncpy(char *dst, const char *src, uint32_t len)
{
    strncpy(dst, src, len);
}

int32_t env_strncmp(char *dst, const char *src, uint32_t len)
{
    return (strncmp(dst, src, len));
}

void env_mb(void)
{
    __asm__ volatile("": : :"memory");
}

void env_rmb(void)
{
    __asm__ volatile("": : :"memory");
}

void env_wmb(void)
{
    __asm__ volatile("": : :"memory");
}

uint32_t env_map_vatopa(void *address)
{
    return platform_vatopa(address);
}

void *env_map_patova(uint32_t address)
{
    return platform_patova(address);
}

int32_t env_create_mutex(void **lock, int32_t count)
{
    sema_t* sema = env_allocate_memory(sizeof(sema_t));
    if (sema == NULL)
        return -1;
    sema_create(sema, count);
    *lock = (void*)sema;
    return 0;
}

void env_delete_mutex(void *lock)
{
    sema_t* sema = (sema_t*) lock;
    sema_destroy(sema);
    free(sema);
}

void env_lock_mutex(void *lock)
{
    sema_t *sema = (sema_t*) lock;
    sema_wait(sema); 
}

void env_unlock_mutex(void *lock)
{
    sema_t *sema = (sema_t*) lock;
    sema_post(sema); 
}

int32_t env_create_sync_lock(void **lock, int32_t state)
{
   (void) lock;
   (void) state;
   DEBUG("%s",__FUNCTION__);
   return 0;
}

void env_sleep_msec(uint32_t num_msec)
{
    platform_time_delay(num_msec);
}

void env_register_isr(uint32_t vector_id, void *data)
{
    if (vector_id < ISR_COUNT)
    {
        isr_table[vector_id].data = data;
    }
}

void env_unregister_isr(uint32_t vector_id)
{
    if (vector_id < ISR_COUNT)
    {
        isr_table[vector_id].data = NULL;
    }
}

void env_enable_interrupt(uint32_t vector_id)
{
    platform_interrupt_enable(vector_id);
}

void env_disable_interrupt(uint32_t vector_id)
{
    platform_interrupt_disable(vector_id);
}

void env_map_memory(uint32_t pa, uint32_t va, uint32_t size, uint32_t flags)
{
    (void) pa;
    (void) va;
    (void) size;
    (void) flags;
    DEBUG("%s",__FUNCTION__);

}

void env_disable_cache(void)
{
    DEBUG("%s",__FUNCTION__);

}

/*========================================================= */
/* Util data / functions  */

void env_isr(uint32_t vector)
{
    struct isr_info *info;
    RL_ASSERT(vector < ISR_COUNT);
    if (vector < ISR_COUNT)
    {
        info = &isr_table[vector];
        virtqueue_notification((struct virtqueue *)info->data);
    }
}


int32_t env_create_queue(void **queue, int32_t length, int32_t element_size)
{
    (void)queue;
    (void)length;
    (void)element_size;
    msg_t *msg = NULL;

    msg = env_allocate_memory(sizeof(msg_t) * length);

    if (msg == NULL) {
        DEBUG("[rpmsg]: Cannot allocate memory for queue\n");
        return -1;
    }
    /* Initialize the queue */
    msg_init_queue(msg, length);
    
    *queue = (void*)msg;
    return 0;
}

void env_delete_queue(void *queue)
{
    (void) queue;
    DEBUG("%s",__FUNCTION__);
}

typedef struct
{
    uint32_t src;
    void *data;
    short int len;
} rpmsg_queue_rx_cb_data_t;

int32_t env_put_queue(void *queue, void *msg, uint32_t timeout_ms)
{
    (void) timeout_ms;
    (void) queue;
    msg_t m;
    uint32_t* p =  env_allocate_memory(sizeof(rpmsg_queue_rx_cb_data_t));
    if (p == NULL) {
        printf("put failed\n");
        return 0;
    }
    m.type = 0x3456;
    m.content.ptr = p;

    /*printf("msg.len:%d\n", ((rpmsg_queue_rx_cb_data_t*)msg)->len);
    printf("msg.data:%p\n", ((rpmsg_queue_rx_cb_data_t*)msg)->data);
    printf("msg:%p\n", msg);
    printf("ptr:%p\n", m.content.ptr);*/
    memcpy( m.content.ptr, msg, sizeof(rpmsg_queue_rx_cb_data_t));
    if(env_in_isr()) {
        msg_send_int(&m, mpid);
        thread_yield();
        return 1;
    }
    else
        puts("noint");
   // msg_queue->content.ptr = msg;
    //msg_send(msg_queue, );
    return 0;
}

int32_t env_get_queue(void *queue, void *msg, uint32_t timeout_ms)
{

    (void) timeout_ms;
    (void) queue;
    msg_t m;
    msg_receive(&m);
    //printf("len:%d\n",((rpmsg_queue_rx_cb_data_t*)m.content.ptr)->len);
    //printf("ptr:%p\n",m.content.ptr);
    memcpy(msg, m.content.ptr,sizeof(rpmsg_queue_rx_cb_data_t));
   // puts("done");
    return 1;
}


int32_t env_get_current_queue_size(void *queue)
{
    (void) queue;
    DEBUG("%s\n",__FUNCTION__);
    return 0;
}
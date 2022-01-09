/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include <assert.h>
#include <sys/uio.h>
#include <inttypes.h>

#include "net/eui64.h"
#include "net/ipv6/addr.h"
#include "net/netdev.h"
#include "net/netopt.h"
#include "utlist.h"
#include "thread.h"

#define ENABLE_DEBUG                1
#include "debug.h"

#define NRF_MODEM_NETDEV_NAME            "nrf_modem_netdev"
#define NRF_MODEM_NETDEV_PRIO            (THREAD_PRIORITY_MAIN - 4)
#define NRF_MODEM_NETDEV_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#define NRF_MODEM_NETDEV_QUEUE_LEN       (8)
#define NRF_MODEM_NETDEV_MSG_TYPE_EVENT 0x1235

#define WPAN_IFNAME1 'W'


static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _stack[NRF_MODEM_NETDEV_STACKSIZE];
static msg_t _queue[NRF_MODEM_NETDEV_QUEUE_LEN];

static void *_event_loop(void *arg);

int nrf_modem_netdev_init(void)
{
    /* start multiplexing thread (only one needed) */
    if (_pid <= KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, NRF_MODEM_NETDEV_STACKSIZE,
                             NRF_MODEM_NETDEV_PRIO, THREAD_CREATE_STACKTEST,
                             _event_loop, NULL, NRF_MODEM_NETDEV_NAME);
        if (_pid <= 0) {
            return -1;
        }
    }

    return 0;
}

static void *_event_loop(void *arg)
{
    (void)arg;
    DEBUG_PUTS("\n[netdev_thread]: Starting thread...");
    msg_init_queue(_queue, NRF_MODEM_NETDEV_QUEUE_LEN);
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == NRF_MODEM_NETDEV_MSG_TYPE_EVENT) {
            netdev_t *dev = msg.content.ptr;
            dev->driver->isr(dev);
        }
    }
    return NULL;
}
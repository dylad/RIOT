/*
 * Copyright (C) 2019 Mesotic SAS <dylan.laduranty@mesotic.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_ns.h"

#include "thread.h"
#include "shell.h"
#define ENABLE_DEBUG (1)
#include "debug.h"

static char _recv_stack[THREAD_STACKSIZE_DEFAULT];

#define RPMSG_LITE_LINK_ID (RL_PLATFORM_IMX8MQ_M4_USER_LINK_ID)
#define RPMSG_LITE_SHMEM_BASE 0xB8000000
#define RPMSG_LITE_NS_ANNOUNCE_STRING "rpmsg-openamp-demo-channel"
#define RPMSG_LITE_MASTER_IS_LINUX

static char linux_riotos_msg[13];
#ifndef LOCAL_EPT_ADDR
#define LOCAL_EPT_ADDR (30)
#endif
#define APP_RPMSG_READY_EVENT_DATA (1)
kernel_pid_t mpid;

typedef struct rpmsg_msg
{
    uint32_t DATA;
} volatile rpmsg_msg_t, *rpmsg_msg_ptr;

volatile rpmsg_msg_t msg = {0};

void rpmsg_cb(uint32_t new_ept, const char *new_ept_name,
                            uint32_t flags, void *user_data)
{
    (void) new_ept;
    (void) new_ept_name;
    (void) flags;
    (void) user_data;
}

void *_recv_thread(void *arg)
{
    volatile unsigned long remote_addr;
    struct rpmsg_lite_endpoint *volatile ep;
    volatile rpmsg_queue_handle rpmsg_queue;
    struct rpmsg_lite_instance *volatile rpmsg_dev;
    volatile rpmsg_ns_handle ns_handle;
    /* Setup */

   (void) arg;
    printf("[rpmsg]: Initialization...");
    rpmsg_dev = rpmsg_lite_remote_init((void *)RPMSG_LITE_SHMEM_BASE,
                                       RPMSG_LITE_LINK_ID, RL_NO_FLAGS);

    if(rpmsg_dev == NULL) {
        puts("failed");
    }
    else {
        puts("OK");
    }
    DEBUG("[rpmsg]: wait for link...\n");
    while (!rpmsg_lite_is_link_up(rpmsg_dev)) {}
    DEBUG("[rpmsg]: Link is up\n");

    rpmsg_queue  = rpmsg_queue_create(rpmsg_dev);
    DEBUG("[rpmsg]: Create rpmsg queue\n");
    ep = rpmsg_lite_create_ept(rpmsg_dev, LOCAL_EPT_ADDR, rpmsg_queue_rx_cb,
                               rpmsg_queue);
    DEBUG("[rpmsg]: Create rpmsg endpoint\n");
    ns_handle = rpmsg_ns_bind(rpmsg_dev, rpmsg_cb, NULL);
    platform_time_delay(1000);
    rpmsg_ns_announce(rpmsg_dev, ep, RPMSG_LITE_NS_ANNOUNCE_STRING, RL_NS_CREATE);
    DEBUG("[rpmsg]: Nameservice announce sent\n");

    rpmsg_queue_recv(rpmsg_dev, rpmsg_queue, (unsigned long *)&remote_addr,
                     linux_riotos_msg, sizeof(linux_riotos_msg), NULL, RL_BLOCK);

    printf("[rpmsg]: Host sent \"%s\"\n", linux_riotos_msg);
  
    while(msg.DATA <= 100) {
        DEBUG("Waiting for ping...\n");
        rpmsg_queue_recv(rpmsg_dev, rpmsg_queue, (unsigned long *)&remote_addr,
                         (char *)&msg, sizeof(rpmsg_msg_t), NULL, RL_BLOCK);
        printf("[rpmsg]: send \"%ld\" to host\n", msg.DATA);
        msg.DATA++;
        DEBUG("Sending pong...\n");
        rpmsg_lite_send(rpmsg_dev, ep, remote_addr, (char *)&msg, sizeof(rpmsg_msg_t), RL_BLOCK);
    }

    puts("Ping pong done, deinitializing...");

    rpmsg_lite_destroy_ept(rpmsg_dev, ep);
    ep = NULL;
    rpmsg_queue_destroy(rpmsg_dev, rpmsg_queue);
    rpmsg_queue = NULL;
    rpmsg_ns_unbind(rpmsg_dev, ns_handle);
    rpmsg_lite_deinit(rpmsg_dev);
    msg.DATA = 0;

    return 0;
}

int main(void)
{
    puts("Creating thread for RPmsg handling");
    mpid = thread_create(_recv_stack, sizeof(_recv_stack),
                         THREAD_PRIORITY_MAIN-1, THREAD_CREATE_STACKTEST,
                         _recv_thread, NULL, "recv thread");

    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
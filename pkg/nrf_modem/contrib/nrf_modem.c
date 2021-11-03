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
 * @author Dylan Laduranty <dylan.laduranty@mesotic.com
 */

#include "kernel_defines.h"
#include <stdio.h>

extern int nrf_modem_netdev_init(void);
void nrf_modem_bootstrap(void)
{
    /* Initialize netdev thread */
    int res = nrf_modem_netdev_init();
    if (res != 0) {
        puts("auto init failed");
    } else {
        puts("auto init done");
    }

}

/** @} */

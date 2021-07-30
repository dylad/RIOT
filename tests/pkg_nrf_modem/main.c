/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Nordic nrf_modem example application for nRF9160-based MCUs
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "nrf_modem_os.h"
#include "nrf_modem.h"


int main(void)
{
    puts("nrf modem test application\n");

    int ret;
    /* Initialize Modem librairy */
    ret = nrf_modem_init(NULL, NORMAL_MODE);
    if (ret != 0) {
        puts("Init failed");
        return ret;
    }
    puts("lib init");
   
    return 0;
}

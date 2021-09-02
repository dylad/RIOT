/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_polarfire
 * @{
 *
 * @file
 * @brief           CPU specific configuration options
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

/* TODO Update vendor CPU core header to Polarfire */
//#include "vendor/gd32vf103_core.h"
#include "vendor/mss_address_map.h"
#include "vendor/mss_sysreg.h"
#include "vendor/mss_gpio.h"
#include "vendor/mss_uart.h"
#include "cpu_conf_common.h"

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   Polarfire SoC doesn't use the MIE CSR
 */
#define HAVE_CSR_MIE                    (0)


#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */

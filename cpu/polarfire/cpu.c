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
 * @brief           Polarfire CPU initialization
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */
#include "stdio_uart.h"
#include "periph/init.h"
#include "irq_arch.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

extern void __libc_init_array(void);

void cpu_init(void)
{
    //gd32vf103_clock_init();
    /* Common RISC-V initialization */
    riscv_init();

/* Disable stdio for some time */
#if 0
    stdio_init();
    periph_init();
#endif
}

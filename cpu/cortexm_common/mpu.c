/*
 * Copyright (C) 2016 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file        mpu.c
 * @brief       Cortex-M Memory Protection Unit (MPU) Driver
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include "cpu.h"
#include "mpu.h"

int mpu_disable(void) {
#if __MPU_PRESENT
    ARM_MPU_Disable();
    return 0;
#else
    return -1;
#endif
}

int mpu_enable(void) {
#if __MPU_PRESENT
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
    return 0;
#else
    return -1;
#endif
}

bool mpu_enabled(void) {
#if __MPU_PRESENT
    return (MPU->CTRL & MPU_CTRL_ENABLE_Msk) != 0;
#else
    return false;
#endif
}

int mpu_configure_v7(uint_fast8_t region, uintptr_t base, uint_fast32_t attr) {
#if __MPU_PRESENT && (defined(__ARM_ARCH_7M_MAIN__) && defined(__ARM_ARCH_7M_BASE__))
    MPU->RNR  = region;
    MPU->RBAR = base & MPU_RBAR_ADDR_Msk;
    MPU->RASR = attr | MPU_RASR_ENABLE_Msk;

    return 0;
#else
    (void)region;
    (void)base;
    (void)attr;
    return -1;
#endif
}

int mpu_configure_v8(uint_fast8_t region, uintptr_t base, uintptr_t end,
                     uint_fast32_t attr)
{
#if __MPU_PRESENT && (defined(__ARM_ARCH_8M_MAIN__) && defined(__ARM_ARCH_8M_BASE__))
    ARM_MPU_SetRegionEx(MPU, region, base | attr, end | MPU_RLAR_EN_Msk);
    return 0;
#else
    (void)region;
    (void)base;
    (void)end;
    (void)attr;
    return -1;
#endif
}

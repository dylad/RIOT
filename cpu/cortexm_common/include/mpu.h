/*
 * Copyright (C) 2016 Loci Controls Inc.
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file        mpu.h
 * @brief       Cortex-M Memory Protection Unit (MPU) Driver Header File
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#ifndef MPU_H
#define MPU_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Number of MPU regions available (will vary depending on the Cortex-M version)
 */
#define MPU_NUM_REGIONS ( (MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos )

/**
 * @brief Access Permission words for MPU in ARMv7 architecture
 */
enum {
    AP_NO_NO_V7 = 0, /**< no access for all levels */
    AP_RW_NO_V7 = 1, /**< read/write for privileged level, no access from user level */
    AP_RW_RO_V7 = 2, /**< read/write for privileged level, read-only for user level */
    AP_RW_RW_V7 = 3, /**< read/write for all levels */
    AP_RO_NO_V7 = 5, /**< read-only for privileged level, no access from user level */
    AP_RO_RO_V7 = 6, /**< read-only for all levels */
};

/**
 * @brief Access Permission words for MPU in ARMv8 architecture
 */
enum {
    AP_RW_NO_V8 = 0, /**< read/write for privileged level, no acces from user level */
    AP_RW_RW_V8 = 1, /**< read/write for all levels */
    AP_RO_NO_V8 = 2, /**< read-only for privileged level  */
    AP_RO_RO_V8 = 3, /**< read-only for all levels */
};

/**
 * @brief MPU region sizes
 */
enum {
    MPU_SIZE_32B  =  4, /**<  32 bytes     */
    MPU_SIZE_64B  =  5, /**<  64 bytes     */
    MPU_SIZE_128B =  6, /**< 128 bytes     */
    MPU_SIZE_256B =  7, /**< 256 bytes     */
    MPU_SIZE_512B =  8, /**< 512 bytes     */
    MPU_SIZE_1K   =  9, /**<   1 kilobytes */
    MPU_SIZE_2K   = 10, /**<   2 kilobytes */
    MPU_SIZE_4K   = 11, /**<   4 kilobytes */
    MPU_SIZE_8K   = 12, /**<   8 kilobytes */
    MPU_SIZE_16K  = 13, /**<  16 kilobytes */
    MPU_SIZE_32K  = 14, /**<  32 kilobytes */
    MPU_SIZE_64K  = 15, /**<  64 kilobytes */
    MPU_SIZE_128K = 16, /**< 128 kilobytes */
    MPU_SIZE_256K = 17, /**< 256 kilobytes */
    MPU_SIZE_512K = 18, /**< 512 kilobytes */
    MPU_SIZE_1M   = 19, /**<   1 megabytes */
    MPU_SIZE_2M   = 20, /**<   2 megabytes */
    MPU_SIZE_4M   = 21, /**<   4 megabytes */
    MPU_SIZE_8M   = 22, /**<   8 megabytes */
    MPU_SIZE_16M  = 23, /**<  16 megabytes */
    MPU_SIZE_32M  = 24, /**<  32 megabytes */
    MPU_SIZE_64M  = 25, /**<  64 megabytes */
    MPU_SIZE_128M = 26, /**< 128 megabytes */
    MPU_SIZE_256M = 27, /**< 256 megabytes */
    MPU_SIZE_512M = 28, /**< 512 megabytes */
    MPU_SIZE_1G   = 29, /**<   1 gigabytes */
    MPU_SIZE_2G   = 30, /**<   2 gigabytes */
    MPU_SIZE_4G   = 31, /**<   4 gigabytes */
};

/**
 * @brief convert a region size code to a size in bytes
 *
 * @param[in]   size    region size code, e.g. MPU_SIZE_32B
 *
 * @return region size in bytes
 */
#define MPU_SIZE_TO_BYTES(size) ( (uintptr_t)1 << ((size) + 1) )

/**
 * @brief generate an MPU attribute word suitable for writing to the RASR register
 *
 * @param[in]   xn      eXecute Never flag (forbids instruction fetches)
 * @param[in]   ap      Access Permission word, e.g. AP_RO_RO
 * @param[in]   tex     Type Extension Field
 * @param[in]   c       Cacheable bit
 * @param[in]   b       Bufferable bit
 * @param[in]   s       Sub-Region Disable (SRD) field
 * @param[in]   size    region size code, e.g. MPU_SIZE_32B
 *
 * @return combined region attribute word
 */
static inline uint32_t MPU_ATTR_V7(
    uint32_t xn,
    uint32_t ap,
    uint32_t tex,
    uint32_t c,
    uint32_t b,
    uint32_t s,
    uint32_t size)
{
    return
        (xn   << 28) |
        (ap   << 24) |
        (tex  << 19) |
        (s    << 18) |
        (c    << 17) |
        (b    << 16) |
        (size <<  1);
}

/**
 * @brief generate an MPU attribute word suitable for writing to the RBAR register
 *
 * @param[in]   sh      Shareability
 * @param[in]   ap      Access Permission word, e.g. AP_RO_RO
 * @param[in]   xn      eXecute Never flag (forbids instruction fetches)
 *
 * @return combined region attribute word
 */
static inline uint32_t MPU_ATTR_V8(
    uint32_t sh,  
    uint32_t ap,
    uint32_t xn)
{
    return
        (sh   << MPU_RBAR_SH_Pos) |
        (ap   << MPU_RBAR_AP_Pos) |
        (xn   << MPU_RBAR_XN_Pos);
}

/**
 * @brief disable the MPU
 *
 * @return 0 on success
 * @return <0 on failure or no MPU present
 */
int mpu_disable(void);

/**
 * @brief enable the MPU
 *
 * @return 0 on success
 * @return <0 on failure or no MPU present
 */
int mpu_enable(void);

/**
 * @brief test if the MPU is enabled
 *
 * @return true if enabled
 * @return false if disabled
 */
bool mpu_enabled(void);

/**
 * @brief configure the base address and attributes for an MPU region
 *        in ARMv7 architecture
 *
 * @param[in]   region  MPU region to configure (0 <= @p region < MPU_NUM_REGIONS)
 * @param[in]   base    base address in RAM (aligned to the size specified within @p attr)
 * @param[in]   attr    attribute word generated by MPU_ATTR_V7()
 *
 * @return 0 on success
 * @return <0 on failure or no MPU present
 */
int mpu_configure_v7(uint_fast8_t region, uintptr_t base, uint_fast32_t attr);

/**
 * @brief configure the base address and attributes for an MPU region
 *        in ARMv8 architecture
 *
 * @param[in]   region  MPU region to configure (0 <= @p region < MPU_NUM_REGIONS)
 * @param[in]   base    base address in RAM
 * @param[in]   end     end  address in RAM
 * @param[in]   attr    attribute word generated by MPU_ATTR_V8()
 *
 * @return 0 on success
 * @return <0 on failure or no MPU present
 */
int mpu_configure_v8(uint32_t region, uint32_t rbar, uint32_t rlar,
                     uint32_t attr);

#ifdef __cplusplus
}
#endif

#endif /* MPU_H */

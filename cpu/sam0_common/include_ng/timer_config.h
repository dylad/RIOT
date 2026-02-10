/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_sam0_common
 * @brief           Generic Timer MCLK masks.
 * @{
 *
 * @author          Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Automatically generated helper defines
 * @{
 */
#ifdef MCLK_APBAMASK_TC0_Msk
#  define MCLK_TC0        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TC0_MASK   (MCLK_APBAMASK_TC0_Msk)
#endif
#ifdef MCLK_APBBMASK_TC0_Msk
#  define MCLK_TC0        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TC0_MASK   (MCLK_APBBMASK_TC0_Msk)
#endif
#ifdef MCLK_APBCMASK_TC0_Msk
#  define MCLK_TC0        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TC0_MASK   (MCLK_APBCMASK_TC0_Msk)
#endif
#ifdef MCLK_APBDMASK_TC0_Msk
#  define MCLK_TC0        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TC0_MASK   (MCLK_APBDMASK_TC0_Msk)
#endif

#ifdef MCLK_APBAMASK_TC1_Msk
#  define MCLK_TC1        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TC1_MASK   (MCLK_APBAMASK_TC1_Msk)
#endif
#ifdef MCLK_APBBMASK_TC1_Msk
#  define MCLK_TC1        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TC1_MASK   (MCLK_APBBMASK_TC1_Msk)
#endif
#ifdef MCLK_APBCMASK_TC1_Msk
#  define MCLK_TC1        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TC1_MASK   (MCLK_APBCMASK_TC1_Msk)
#endif
#ifdef MCLK_APBDMASK_TC1_Msk
#  define MCLK_TC1        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TC1_MASK   (MCLK_APBDMASK_TC1_Msk)
#endif

#ifdef MCLK_APBAMASK_TC2_Msk
#  define MCLK_TC2        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TC2_MASK   (MCLK_APBAMASK_TC2_Msk)
#endif
#ifdef MCLK_APBBMASK_TC2_Msk
#  define MCLK_TC2        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TC2_MASK   (MCLK_APBBMASK_TC2_Msk)
#endif
#ifdef MCLK_APBCMASK_TC2_Msk
#  define MCLK_TC2        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TC2_MASK   (MCLK_APBCMASK_TC2_Msk)
#endif
#ifdef MCLK_APBDMASK_TC2_Msk
#  define MCLK_TC2        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TC2_MASK   (MCLK_APBDMASK_TC2_Msk)
#endif

#ifdef MCLK_APBAMASK_TC3_Msk
#  define MCLK_TC3        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TC3_MASK   (MCLK_APBAMASK_TC3_Msk)
#endif
#ifdef MCLK_APBBMASK_TC3_Msk
#  define MCLK_TC3        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TC3_MASK   (MCLK_APBBMASK_TC3_Msk)
#endif
#ifdef MCLK_APBCMASK_TC3_Msk
#  define MCLK_TC3        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TC3_MASK   (MCLK_APBCMASK_TC3_Msk)
#endif
#ifdef MCLK_APBDMASK_TC3_Msk
#  define MCLK_TC3        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TC3_MASK   (MCLK_APBDMASK_TC3_Msk)
#endif

#ifdef MCLK_APBAMASK_TC4_Msk
#  define MCLK_TC4        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TC4_MASK   (MCLK_APBAMASK_TC4_Msk)
#endif
#ifdef MCLK_APBBMASK_TC4_Msk
#  define MCLK_TC4        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TC4_MASK   (MCLK_APBBMASK_TC4_Msk)
#endif
#ifdef MCLK_APBCMASK_TC4_Msk
#  define MCLK_TC4        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TC4_MASK   (MCLK_APBCMASK_TC4_Msk)
#endif
#ifdef MCLK_APBDMASK_TC4_Msk
#  define MCLK_TC4        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TC4_MASK   (MCLK_APBDMASK_TC4_Msk)
#endif

#ifdef MCLK_APBAMASK_TC5_Msk
#  define MCLK_TC5        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TC5_MASK   (MCLK_APBAMASK_TC5_Msk)
#endif
#ifdef MCLK_APBBMASK_TC5_Msk
#  define MCLK_TC5        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TC5_MASK   (MCLK_APBBMASK_TC5_Msk)
#endif
#ifdef MCLK_APBCMASK_TC5_Msk
#  define MCLK_TC5        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TC5_MASK   (MCLK_APBCMASK_TC5_Msk)
#endif
#ifdef MCLK_APBDMASK_TC5_Msk
#  define MCLK_TC5        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TC5_MASK   (MCLK_APBDMASK_TC5_Msk)
#endif

#ifdef MCLK_APBAMASK_TC6_Msk
#  define MCLK_TC6        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TC6_MASK   (MCLK_APBAMASK_TC6_Msk)
#endif
#ifdef MCLK_APBBMASK_TC6_Msk
#  define MCLK_TC6        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TC6_MASK   (MCLK_APBBMASK_TC6_Msk)
#endif
#ifdef MCLK_APBCMASK_TC6_Msk
#  define MCLK_TC6        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TC6_MASK   (MCLK_APBCMASK_TC6_Msk)
#endif
#ifdef MCLK_APBDMASK_TC6_Msk
#  define MCLK_TC6        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TC6_MASK   (MCLK_APBDMASK_TC6_Msk)
#endif

#ifdef MCLK_APBAMASK_TC7_Msk
#  define MCLK_TC7        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TC7_MASK   (MCLK_APBAMASK_TC7_Msk)
#endif
#ifdef MCLK_APBBMASK_TC7_Msk
#  define MCLK_TC7        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TC7_MASK   (MCLK_APBBMASK_TC7_Msk)
#endif
#ifdef MCLK_APBCMASK_TC7_Msk
#  define MCLK_TC7        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TC7_MASK   (MCLK_APBCMASK_TC7_Msk)
#endif
#ifdef MCLK_APBDMASK_TC7_Msk
#  define MCLK_TC7        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TC7_MASK   (MCLK_APBDMASK_TC7_Msk)
#endif

#ifdef MCLK_APBAMASK_TCC0_Msk
#  define MCLK_TCC0        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TCC0_MASK   (MCLK_APBAMASK_TCC0_Msk)
#endif
#ifdef MCLK_APBBMASK_TCC0_Msk
#  define MCLK_TCC0        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TCC0_MASK   (MCLK_APBBMASK_TCC0_Msk)
#endif
#ifdef MCLK_APBCMASK_TCC0_Msk
#  define MCLK_TCC0        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TCC0_MASK   (MCLK_APBCMASK_TCC0_Msk)
#endif
#ifdef MCLK_APBDMASK_TCC0_Msk
#  define MCLK_TCC0        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TCC0_MASK   (MCLK_APBDMASK_TCC0_Msk)
#endif

#ifdef MCLK_APBAMASK_TCC1_Msk
#  define MCLK_TCC1        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TCC1_MASK   (MCLK_APBAMASK_TCC1_Msk)
#endif
#ifdef MCLK_APBBMASK_TCC1_Msk
#  define MCLK_TCC1        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TCC1_MASK   (MCLK_APBBMASK_TCC1_Msk)
#endif
#ifdef MCLK_APBCMASK_TCC1_Msk
#  define MCLK_TCC1        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TCC1_MASK   (MCLK_APBCMASK_TCC1_Msk)
#endif
#ifdef MCLK_APBDMASK_TCC1_Msk
#  define MCLK_TCC1        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TCC1_MASK   (MCLK_APBDMASK_TCC1_Msk)
#endif

#ifdef MCLK_APBAMASK_TCC2_Msk
#  define MCLK_TCC2        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TCC2_MASK   (MCLK_APBAMASK_TCC2_Msk)
#endif
#ifdef MCLK_APBBMASK_TCC2_Msk
#  define MCLK_TCC2        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TCC2_MASK   (MCLK_APBBMASK_TCC2_Msk)
#endif
#ifdef MCLK_APBCMASK_TCC2_Msk
#  define MCLK_TCC2        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TCC2_MASK   (MCLK_APBCMASK_TCC2_Msk)
#endif
#ifdef MCLK_APBDMASK_TCC2_Msk
#  define MCLK_TCC2        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TCC2_MASK   (MCLK_APBDMASK_TCC2_Msk)
#endif

#ifdef MCLK_APBAMASK_TCC3_Msk
#  define MCLK_TCC3        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TCC3_MASK   (MCLK_APBAMASK_TCC3_Msk)
#endif
#ifdef MCLK_APBBMASK_TCC3_Msk
#  define MCLK_TCC3        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TCC3_MASK   (MCLK_APBBMASK_TCC3_Msk)
#endif
#ifdef MCLK_APBCMASK_TCC3_Msk
#  define MCLK_TCC3        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TCC3_MASK   (MCLK_APBCMASK_TCC3_Msk)
#endif
#ifdef MCLK_APBDMASK_TCC3_Msk
#  define MCLK_TCC3        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TCC3_MASK   (MCLK_APBDMASK_TCC3_Msk)
#endif

#ifdef MCLK_APBAMASK_TCC4_Msk
#  define MCLK_TCC4        (&MCLK_REGS->MCLK_APBAMASK)
#  define MCLK_TCC4_MASK   (MCLK_APBAMASK_TCC4_Msk)
#endif
#ifdef MCLK_APBBMASK_TCC4_Msk
#  define MCLK_TCC4        (&MCLK_REGS->MCLK_APBBMASK)
#  define MCLK_TCC4_MASK   (MCLK_APBBMASK_TCC4_Msk)
#endif
#ifdef MCLK_APBCMASK_TCC4_Msk
#  define MCLK_TCC4        (&MCLK_REGS->MCLK_APBCMASK)
#  define MCLK_TCC4_MASK   (MCLK_APBCMASK_TCC4_Msk)
#endif
#ifdef MCLK_APBDMASK_TCC4_Msk
#  define MCLK_TCC4        (&MCLK_REGS->MCLK_APBDMASK)
#  define MCLK_TCC4_MASK   (MCLK_APBDMASK_TCC4_Msk)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

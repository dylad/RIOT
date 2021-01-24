/*
 * Copyright (C) 2019-2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 *
 *
 */

/**
 * @defgroup    usbus_msc USBUS Mass Storage Class functions
 * @ingroup     usb
 *
 * @{
 *
 * @file
 * @brief       USB Mass Storage Class functions definitions
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef USB_MSC_H
#define USB_MSC_H

#include <stdint.h>
#include "usb/usbus.h"

#ifdef __cplusplus
extern "c" {
#endif

/**
 * @name USB Mass Storage Class subclass definitions
 *
 * @see Table 1 — SubClass Codes Mapped to Command Block Specifications
 * from Universal Serial Bus Mass Storage Class Specification Overview 1.4
 * @{
 */
#define USB_MSC_SUBCLASS_SCSI         0x00 /**< SCSI command set not reported */
#define USB_MSC_SUBCLASS_RBC          0x01 /**< RBC allocated by USB-IF */
#define USB_MSC_SUBCLASS_MMC5         0x02 /**< MMC5 allocated by USB-IF */
#define USB_MSC_SUBCLASS_UFI          0x04 /**< Interface Floppy Disk Drives */
#define USB_MSC_SUBCLASS_SCSI_TCS     0x06 /**< SCSI transparent command set */
#define USB_MSC_SUBCLASS_LSDFS        0x07 /**< Early negotiation acces */
#define USB_MSC_SUBCLASS_IEEE1667     0x08 /**< IEEE1677 allocated by USB-IF */
#define USB_MSC_SUBCLASS_VENDOR       0xFF /**< Vendor Specific */
/** @} */

/**
 * @name USB Mass Storage Class protocol definitions
 *
 * @see Table 2 — Mass Storage Transport Protocol
 * from Universal Serial Bus Mass Storage Class Specification Overview 1.4
 * @{
 */
#define USB_MSC_PROTOCOL_CBI_CCI      0x00  /**< CBI transport with command completion interrupt*/
#define USB_MSC_PROTOCOL_CBI_NO_CCI   0x01  /**< CBI transport without command completion interrupt */
#define USB_MSC_PROTOCOL_BBB          0x50  /**< Bulk only (BBB) transport */
#define USB_MSC_PROTOCOL_UAS          0x62  /**< UAS allocated by USB-IF */
#define USB_MSC_PROTOCOL_VENDOR       0xFF  /**< Vendor Specific */
/** @} */

/**
 * @brief   Mass Storage Class descriptor
 *
 * @see     Table 4 — Mass Storage Class Specific Descriptors
 * from Universal Serial Bus Mass Storage Class Specification Overview 1.4
 */
#define USB_TYPE_DESCRIPTOR_MSC         0x24

/**
 * @brief   Command Block Wrapper flags
 *
 * @see     Chap 5.1 — Command Block Wrapper (CBW)
 * from Universal Serial Bus Mass Storage Class Bulk-Only Transport
 */
#define USB_MSC_CBW_FLAG_IN             0x80    /**< Indicate Device to Host direction */

/**
 * @name USB Mass Storage Class request codes
 *
 * @see Table 3 — Mass Storage Request Codes
 * from Universal Serial Bus Mass Storage Class Specification Overview 1.4
 * @{
 */
#define USB_MSC_SETUP_REQ_ADSC      0x01     /**< Accept Device-Specific Command request */
#define USB_MSC_SETUP_REQ_GET_REQ   0xFC     /**< Get Request */
#define USB_MSC_SETUP_REQ_PUT_REQ   0xFD     /**< Put Request */
#define USB_MSC_SETUP_REQ_GML       0xFE     /**< Get Max LUN request */
#define USB_MSC_SETUP_REQ_BOMSR     0xFF     /**< Bulk-Only Mass Storage Reset request */
/** @} */

typedef struct __attribute__((packed)) {
    uint8_t length;
    uint8_t type;
    uint8_t subtype;
} usb_desc_msc_t;

#ifdef __cplusplus
}
#endif

#endif /* USB_MSC_H */
/** @} */
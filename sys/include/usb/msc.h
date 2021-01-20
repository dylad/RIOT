/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 * 
 *
 */

/**
 * @defgroup    usb_usbus_mass_storage USBUS mass storage functions
 * @ingroup     usb_usbus
 *
 * @{
 *
 * @file
 * @brief       USBUS mass storage
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef USB_MASS_STORAGE_H
#define USB_MASS_STORAGE_H

#include <stdint.h>
#include "usb/usbus.h"

#ifdef __cplusplus
extern "c" {
#endif

#define USB_MSC_SUBCLASS_SCSI         0x00 /**< SCSI command set not reported */
#define USB_MSC_SUBCLASS_RBC          0x01 /**< RBC allocated by USB-IF */
#define USB_MSC_SUBCLASS_MMC5         0x02 /**< MMC5 allocated by USB-IF */
#define USB_MSC_SUBCLASS_UFI          0x04 /**< Interface Floppy Disk Drives */
#define USB_MSC_SUBCLASS_SCSI_TCS     0x06 /**< SCSI transparent command set */
#define USB_MSC_SUBCLASS_LSDFS        0x07 /**< Early negotiation acces */
#define USB_MSC_SUBCLASS_IEEE1667     0x08 /**< IEEE1677 allocated by USB-IF */
#define USB_MSC_SUBCLASS_VENDOR       0xFF /**< Vendor Specific */


#define USB_MSC_PROTOCOL_CBI_CCI      0x00 /**< CBI transport with CCI*/
#define USB_MSC_PROTOCOL_CBI_NO_CCI   0x01 /**< CBI transport without CCI */
#define USB_MSC_PROTOCOL_BBB          0x02 /**< Bulk only transport */
#define USB_MSC_PROTOCOL_UAS          0x03 /**< UAS allocated by USB-IF */
#define USB_MSC_PROTOCOL_VENDOR       0x04 /**< Vendor Specific */
#define USB_MSC_PROTOCOL_BULK_ONLY    0x50

#ifndef USBUS_MSC_BLOCKSIZE
#define USBUS_MSC_BLOCKSIZE           64
#endif /*USBUS_MSC_BLOCKSIZE */

#ifndef USBUS_MSC_BLOCK_NUM
#define USBUS_MSC_BLOCK_NUM 1
#endif /* USBUS_MSC_BLOCK_NUM */


#define USB_TYPE_DESCRIPTOR_MSC         0x24 /**< Mass Storage Class descriptor */

#define USB_MSC_CBW_FLAG_IN             0x80

typedef struct usbus_msc_device usbus_msc_device_t;

typedef struct __attribute__((packed)) {
    uint8_t length;
    uint8_t type;
    uint8_t subtype;
//    uint8_t capabalities;
//    uint8_t data_if;
} usb_desc_msc_t;

typedef struct {
    uint32_t tag;
    size_t len;
    uint8_t status;
} cbw_info_t;

typedef enum {
    WAITING,
    WAIT_FOR_TRANSFER,
    DATA_TRANSFER,
    DATA_TRANSFER_LAST,
    GEN_CSW
} usbus_msc_state_t;

struct usbus_msc_device {
    usbus_handler_t handler_ctrl;
    usbus_interface_t iface;
    usbus_endpoint_t *ep_in;
    usbus_endpoint_t *ep_out;
    usbus_descr_gen_t msc_descr;
    usbus_t *usbus;
    cbw_info_t cmd;
    event_t rx_event;                        /**< Transmit ready event */
    event_t tx_event;
    usbus_msc_state_t state;
    uint8_t *buffer;
    uint32_t block;
    uint16_t block_nb;
    uint16_t block_offset;
};

#ifdef __cplusplus
}
#endif

#endif /* USB_CDC_H */
/** @} */
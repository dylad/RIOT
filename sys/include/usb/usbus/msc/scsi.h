/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#ifndef USBUS_SCSI_H
#define USBUS_SCSI_H

#ifdef __cplusplus
extern "c" {
#endif

#ifndef USBUS_MSC_VENDOR_ID
#define USBUS_MSC_VENDOR_ID "RIOT-OS"
#endif /* USBUS_MSC_VENDOR_ID */

#ifndef USBUS_MSC_PRODUCT_ID
#define USBUS_MSC_PRODUCT_ID "RIOT_MSC_DISK"
#endif /* USBUS_MSC_PRODUCT_ID */

#ifndef USBUS_MSC_PRODUCT_REV
#define USBUS_MSC_PRODUCT_REV " 1.0"
#endif /* USBUS_MSC_PRODUCT_REV */

/**
 * @name USB SCSI Commands
 *
 * @see Table 9 - Packet Commands Supported by ATAPI Block Devices
 * from INF-8070i draft published by SFF
 * @{
 */
#define SCSI_TEST_UNIT_READY            0x00
#define SCSI_REQUEST_SENSE              0x03
#define SCSI_FORMAT_UNIT                0x04
#define SCSI_INQUIRY                    0x12
#define SCSI_MODE_SELECT6               0x15
#define SCSI_MODE_SENSE6                0x1A
#define SCSI_START_STOP_UNIT            0x1B
#define SCSI_MEDIA_REMOVAL              0x1E
#define SCSI_READ_FORMAT_CAPACITIES     0x23
#define SCSI_READ_CAPACITY              0x25
#define SCSI_READ10                     0x28
#define SCCI_READ12                     0xA8
#define SCSI_WRITE10                    0x2A
#define SCSI_WRITE12                    0xAA
#define SCSI_SEEK                       0x2B
#define SCSI_WRITE_AND_VERIFY           0x2E
#define SCSI_VERIFY10                   0x2F
#define SCSI_MODE_SELECT10              0x55
#define SCSI_MODE_SENSE10               0x5A
/** @} */

/**
 * @brief Command Block Wrapper signature
 */
#define SCSI_CBW_SIGNATURE              0x43425355

/**
 * @brief Command Status Wrapper signature
 */
#define SCSI_CSW_SIGNATURE              0x53425355


#define SCSI_INQUIRY_CONNECTED          0x00

/**
 * @name USB SCSI Version list
 * @{
 */
#define SCSI_VERSION_NONE               0x0000
#define SCSI_VERSION_SCSI1              0x0001
#define SCSI_VERSION_SCSI2              0x0002
/** @} */

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t logical_unit;
    uint8_t reserved[4];
    uint8_t pad[6];
} msc_test_unit_pkt_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t removable;
    uint16_t version;
    uint8_t length;
    uint8_t tmp[3];
    uint8_t vendor_id[8];
    uint8_t product_id[16];
    uint8_t product_rev[4];
} msc_inquiry_pkt_t;

typedef struct __attribute__((packed)) {
    uint32_t last_blk;
    uint32_t blk_len;
} msc_read_capa_pkt_t;

/* Bulk-only Command Block Wrapper */
typedef struct __attribute__((packed)) {
    uint32_t signature;
    uint32_t tag;
    uint32_t data_len;
    uint8_t  flags;
    uint8_t  lun;
    uint8_t  cb_len;
    uint8_t  cb[16];
} msc_cbw_buf_t;

/* Bulk-only Command Status Wrapper */
typedef struct __attribute__((packed)) {
    uint32_t signature;
    uint32_t tag;
    uint32_t data_left;
    uint8_t  status;
} msc_csw_buf_t;

typedef struct {
    uint32_t tag;
    size_t len;
    uint8_t status;
} cbw_info_t;

/**
 * @brief Process incoming Command Block Wrapper buffer
 *
 * @param   usbus   USBUS thread to use
 * @param   handler MSC device struct
 * @param   ep      Endpoint pointer to read CBW from
 * @param   len     Size of the received CBW buffer
 */
void scsi_process_cmd(usbus_t *usbus, usbus_handler_t *handler, usbdev_ep_t *ep, size_t len);

/**
 * @brief Generate Command Status Wrapper and send it to the host
 *
 * @param   usbus   USBUS thread to use
 * @param   cmd     struct containing needed informations to generate CBW response
 */
void scsi_gen_csw(usbus_handler_t *handler, cbw_info_t cmd);

#ifdef __cplusplus
}
#endif

#endif /* USB_SCSI_H */
/** @} */
/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usb_scsi Mass storage
 * @{
 * @file
 *
 * @author  Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include "thread.h"
#include "msg.h"
#include "mutex.h"
#include "usb/usbus.h"
#include "usb/descriptor.h"
#include "usb/usbopt.h"

#include "usb/msc.h"
#include "usb/usbus/msc/scsi.h"
#include "board.h"
#include "byteorder.h"

#include <string.h>

/* SD-CARD specific */
#include "mtd_sdcard.h"
extern mtd_dev_t *mtd0;

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define VENDOR_ID "RIOT-OS"
#define PRODUCT_ID "RIOT_MSC_DISK"
#define PRODUCT_REV " 1.0"


static void _xmit_ready(usbus_msc_device_t *msc)
{
    usbus_event_post(msc->usbus, &msc->xmit_event);
}

void _scsi_test_unit_ready(usbus_handler_t *handler, usbdev_ep_t *ep,
                           msc_cbw_buf_t *cbw) {

    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    (void)ep;

    DEBUG("flags:0x%x,len:%ld\n", cbw->flags, cbw->data_len);
    if (cbw->data_len != 0) {
        static const usbopt_enable_t enable = USBOPT_ENABLE;
        DEBUG("HOLYSHIT BIG TROUBLE\n\n\n\n");
        if ((cbw->flags & USB_MSC_CBW_FLAG_IN) != 0) {
            usbdev_ep_set(msc->ep_in->ep, USBOPT_EP_STALL, &enable, sizeof(usbopt_enable_t));
        }
        else {
            usbdev_ep_set(msc->ep_out->ep, USBOPT_EP_STALL, &enable, sizeof(usbopt_enable_t));
        }
    }
}
void _scsi_write10(usbus_handler_t *handler, msc_cbw_buf_t *cbw) {
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;

    /* A block is 512 bytes
      offset are counted as block */

    /* Get first block number to read from */
    msc->block = (cbw->cb[2] << 24) |
             (cbw->cb[3] << 16) |
             (cbw->cb[4] <<  8) |
             (cbw->cb[5] <<  0);

    /* Get number of blocks to transfer */
    msc->block_nb = (cbw->cb[7] <<  8) |
                    (cbw->cb[8] <<  0);

    /* FIXME: find a better way to manage this */
    msc->cmd.len = cbw->data_len;
    msc->flags = 1;
   printf("W:offset:%lx, block nb:%d, total:%ld\n",msc->block, msc->block_nb, cbw->data_len);

    return;
}

void _scsi_read10(usbus_handler_t *handler, usbdev_ep_t *ep, msc_cbw_buf_t *cbw) {
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;

    (void) ep;
    /* A block is 512 bytes
      offset are counted as block */

    /* Get first block number to read from */
    msc->block = (cbw->cb[2] << 24) |
             (cbw->cb[3] << 16) |
             (cbw->cb[4] <<  8) |
             (cbw->cb[5] <<  0);

    /* Get number of blocks to transfer */
    msc->block_nb = (cbw->cb[7] <<  8) |
                    (cbw->cb[8] <<  0);

    /* FIXME: find a better way to manage this */
    msc->cmd.len = cbw->data_len;

   printf("offset:%lx, block nb:%d, total:%ld\n",msc->block, msc->block_nb, cbw->data_len);

    if ((cbw->flags & USB_MSC_CBW_FLAG_IN) != 0) {
            _xmit_ready(msc);
    }
    else {
        puts("READ BAD");
    }

    return;
}

void _scsi_inquiry(usbus_handler_t *handler, usbdev_ep_t *ep) {

    (void)ep;
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    msc_inquiry_pkt_t pkt;
    size_t len = sizeof(msc_inquiry_pkt_t);
    memset(&pkt, 0, len);
    /* prepare pkt response */
    pkt.type = SCSI_INQUIRY_CONNECTED;
    pkt.removable = 0x80;
    /* bit flipping */
    //pkt.version = SCSI_VERSION_SCSI1;
    pkt.version = 0x0001;
    pkt.length = len - 4;
    pkt.tmp[0] = 0x80;

    memcpy(&pkt.vendor_id, VENDOR_ID, sizeof(pkt.vendor_id));
    memcpy(&pkt.product_id, PRODUCT_ID, sizeof(pkt.product_id));
    memcpy(&pkt.product_rev, PRODUCT_REV, sizeof(pkt.product_rev));

    /* copy into ep buffer */
    memcpy(msc->ep_in->ep->buf, &pkt, len);
    usbdev_ep_ready(msc->ep_in->ep, len);

    return;
}

void _scsi_read_capacity(usbus_handler_t *handler,  msc_cbw_buf_t *cbw) {

    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    msc_read_capa_pkt_t pkt;
    size_t len = sizeof(msc_read_capa_pkt_t);
    printf("CBW.len:%ld cb_len:%d\n", cbw->data_len, cbw->cb_len);
    pkt.blk_len = byteorder_swapl(SD_HC_BLOCK_SIZE);
    pkt.last_blk = byteorder_swapl(15415296);

    /* copy into ep buffer */
    memcpy(msc->ep_in->ep->buf, &pkt, len);
    usbdev_ep_ready(msc->ep_in->ep, len);
    
}

void _scsi_sense6(usbus_handler_t *handler, msc_cbw_buf_t *cbw) {
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    uint8_t pkt[4];
    size_t len = 4;
    memset(&pkt, 0, len);
    printf("CBW.len:%ld cb_len:%d\n", cbw->data_len, cbw->cb_len);
    pkt[0] = 0x3;

    /* copy into ep buffer */
    memcpy(msc->ep_in->ep->buf, pkt, len);
    usbdev_ep_ready(msc->ep_in->ep, len);
    
}

void _scsi_request_sense(usbus_handler_t *handler, usbdev_ep_t *ep) {
    (void)ep;
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    uint8_t pkt[18];
    size_t len = 18;
    memset(&pkt, 0, len);

    pkt[0] = 0x70;
    pkt[2] = 0x02;
    pkt[7] = 0x0A;
    pkt[12] = 0x30;
    pkt[13] = 0x01;

    /* copy into ep buffer */
    memcpy(msc->ep_in->ep->buf, pkt, len);
    usbdev_ep_ready(msc->ep_in->ep, len);
    
}

int scsi_process_cmd(usbus_t *usbus, usbus_handler_t *handler, usbdev_ep_t *ep, size_t len) {
    (void)usbus;
    (void)len;
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;

    /* store data into specific struct */
    msc_cbw_buf_t *cbw = (msc_cbw_buf_t*) ep->buf;

    /* Check Command Block signature */
    if (cbw->signature != SCSI_CBW_SIGNATURE) {
        printf("Invalid CBW signature:0x%lx, abort\n", cbw->signature);
        msc->cmd.status = -1;
        return -1;
    }

    /* Store command for CSW generation */
    msc->cmd.tag = cbw->tag;
    msc->cmd.status = 0;
    msc->cmd.len = 0;

    switch(cbw->cb[0]) {
        case SCSI_TEST_UNIT_READY:
            puts("SCSI_TEST_UNIT_READY");
            _scsi_test_unit_ready(handler, ep, cbw);
            break;
        case SCSI_REQUEST_SENSE:
            puts("SCSI_REQUEST_SENSE");
            _scsi_request_sense(handler, ep);
            break;
        case SCSI_FORMAT_UNIT:
            puts("TODO: SCSI_FORMAT_UNIT");
            break;
        case SCSI_INQUIRY:
            //puts("SCSI_INQUIRY");
            _scsi_inquiry(handler, ep);
            break;
        case SCSI_START_STOP_UNIT:
            puts("TODO: SCSI_START_STOP_UNIT");
            break;
        case SCSI_MEDIA_REMOVAL:
            msc->cmd.status = 1;
            puts("SCSI_MEDIA_REMOVAL");
            break;
        case SCSI_MODE_SELECT6:
            puts("TODO: SCSI_MODE_SELECT6");
            break;
        case SCSI_MODE_SENSE6:
            puts("SCSI_MODE_SENSE6");
            _scsi_sense6(handler, cbw);
            break;
        case SCSI_MODE_SELECT10:
            puts("TODO: SCSI_MODE_SELECT10");
            break;
        case SCSI_MODE_SENSE10:
            puts("TODO: SCSI_MODE_SENSE10");
            break;
        case SCSI_READ_FORMAT_CAPACITIES:
            puts("TODO: SCSI_READ_FORMAT_CAPACITIES");
            break;
        case SCSI_READ_CAPACITY:
            puts("SCSI_READ_CAPACITY");
            _scsi_read_capacity(handler, cbw);
            break;
        case SCSI_READ10:
            puts("SCSI_READ10");
            _scsi_read10(handler, ep, cbw);
            break;
        case SCSI_WRITE10:
            puts("SCSI_WRITE10");
            _scsi_write10(handler, cbw);
            break;
        case SCSI_VERIFY10:
            puts("TODO: SCSI_VERIFY10");
            break;
        default:
            printf("Unhandled SCSI command:0x%x", cbw->cb[0]);
    }

    return 0;
}

int scsi_gen_csw(usbus_handler_t *handler, cbw_info_t cmd) {
    msc_csw_buf_t csw;
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    memset(&csw, 0, sizeof(msc_csw_buf_t));
    csw.signature = SCSI_CSW_SIGNATURE;
    csw.tag = cmd.tag;
    csw.data_left = cmd.len;
    csw.status = cmd.status;
    memcpy(msc->ep_in->ep->buf, &csw, sizeof(msc_csw_buf_t));
    usbdev_ep_ready(msc->ep_in->ep, sizeof(msc_csw_buf_t));
    return 0;
}

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
#include "kernel_types.h"
#include "msg.h"
#include "mutex.h"
#include "usb/usbus.h"
#include "usb/descriptor.h"
#include "usb/usbopt.h"

#include "usb/msc.h"
#include "usb/usbus/msc/scsi.h"
#include "board.h"

#include <string.h>

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define VENDOR_ID "RIOT-OS"
#define PRODUCT_ID "RIOT_MSC_DISK"
#define PRODUCT_REV " 1.0"

static int _scsi_gen_csw(usbus_handler_t *handler, uint32_t tag, uint8_t status, size_t len);
static const uint8_t CLEUSB[] = {0, 0, 0, 0, 0, 0, 0, 0,
                            'R', 'I', 'O', 'T', '-', 'O', 'S', 0,
                            'a', 'b', 'c', 'd', 'e', 'f', 'g', 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0};

void _scsi_test_unit_ready(usbus_handler_t *handler, usbdev_ep_t *ep,
                           msc_cbw_buf_t *cbw) {

    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    (void)ep;

    if (cbw->data_len != 0) {
        static const usbopt_enable_t enable = USBOPT_ENABLE;
        printf("flags:0x%x,len:%ld\n", cbw->flags,cbw->data_len);
        if ((cbw->flags & USB_MSC_CBW_FLAG_IN) != 0) {
            usbdev_ep_set(msc->ep_in->ep, USBOPT_EP_STALL, &enable, sizeof(usbopt_enable_t));
        }
        else {
            usbdev_ep_set(msc->ep_out->ep, USBOPT_EP_STALL, &enable, sizeof(usbopt_enable_t));
        }
    }
    return;
}

void _scsi_read10(usbus_handler_t *handler, usbdev_ep_t *ep, msc_cbw_buf_t *cbw) {
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    (void)msc;
    uint32_t n;
    uint32_t offset;
    uint32_t nb;

    /* Get offset */
  n = (cbw->cb[2] << 24) |
      (cbw->cb[3] << 16) |
      (cbw->cb[4] <<  8) |
      (cbw->cb[5] <<  0);

  offset = n * 512;
    /* Get number of blocks to transfer */
  n = (cbw->cb[7] <<  8) |
      (cbw->cb[8] <<  0);

   nb = n * 512;

   printf("offset:%lx, block xfer:%ld,total:%ld\n",offset, nb,cbw->data_len);
   uint32_t len = cbw->data_len;
    do {
        len -= ep->len;
        if ((cbw->flags & USB_MSC_CBW_FLAG_IN) != 0) {
            printf("offset:%ld,len:%ld\n",offset,len);
            memcpy(msc->ep_in->ep->buf, &CLEUSB[offset], ep->len);
            usbdev_ep_ready(msc->ep_in->ep, ep->len);
        }
        else {
            puts("READ BAD");
        }
        
        offset += ep->len;
    } while(offset < 512);

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

void _scsi_read_capacity(usbus_handler_t *handler, usbdev_ep_t *ep) {
    (void)ep;
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    //msc_read_capa_pkt_t pkt2;
    uint8_t pkt[8];
    size_t len = sizeof(msc_read_capa_pkt_t);
    memset(pkt, 0, len);
    uint32_t MSC_BlockCount = 1;
    //pkt2.blk_len = 512;
    /*
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    pkt2.last_blk = byteorder_swaps((uint32_t)USBUS_MSC_BLOCK_NUM);
    pkt2.blk_len  = byteorder_swaps((uint32_t)USBUS_MSC_BLOCKSIZE);
#endif
    */
    pkt[ 0] = ((MSC_BlockCount - 1) >> 24) & 0xFF;
    pkt[ 1] = ((MSC_BlockCount - 1) >> 16) & 0xFF;
    pkt[ 2] = ((MSC_BlockCount - 1) >>  8) & 0xFF;
    pkt[ 3] = ((MSC_BlockCount - 1) >>  0) & 0xFF;

    /* Block Length */
    pkt[ 4] = (USBUS_MSC_BLOCKSIZE >> 24) & 0xFF;
    pkt[ 5] = (USBUS_MSC_BLOCKSIZE >> 16) & 0xFF;
    pkt[ 6] = (USBUS_MSC_BLOCKSIZE >>  8) & 0xFF;
    pkt[ 7] = (USBUS_MSC_BLOCKSIZE >>  0) & 0xFF;

    /* copy into ep buffer */
    memcpy(msc->ep_in->ep->buf, pkt, len);
    usbdev_ep_ready(msc->ep_in->ep, len);
    
}

void _scsi_sense6(usbus_handler_t *handler, msc_cbw_buf_t *cbw) {
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    (void)cbw;
    uint8_t pkt[4];
    size_t len = 4;
    memset(&pkt, 0, len);

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

    if (len == sizeof(msc_cbw_buf_t)) {
       // puts("Command Block Wrapper");
    }
    else {
        printf("error receiving, ep->len:%d should be %d\n",len,sizeof(msc_cbw_buf_t));
        return -1;
    }

    /* store data into specific struct */
    msc_cbw_buf_t *cbw = (msc_cbw_buf_t*) ep->buf;

    /* Check Command Block signature */
    if (cbw->signature != SCSI_CBW_SIGNATURE) {
        printf("Invalid CBW signature:0x%lx, abort\n", cbw->signature);
        return -1;
    }

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
            puts("SCSI_INQUIRY");
            _scsi_inquiry(handler, ep);
            break;
        case SCSI_START_STOP_UNIT:
            puts("TODO: SCSI_START_STOP_UNIT");
            break;
        case SCSI_MEDIA_REMOVAL:
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
            puts("TODO: SCSI_READ_CAPACITY");
            _scsi_read_capacity(handler, ep);
            break;
        case SCSI_READ10:
            puts("SCSI_READ10");
            _scsi_read10(handler, ep, cbw);
            break;
        case SCSI_WRITE10:
            puts("TODO: SCSI_WRITE10");
            break;
        case SCSI_VERIFY10:
            puts("TODO: SCSI_VERIFY10");
            break;
        default:
            printf("Unhandled SCSI command:0x%x", cbw->cb[0]);
    }
    _scsi_gen_csw(handler, cbw->tag, 1, 0);
    return 0;
}

int _scsi_gen_csw(usbus_handler_t *handler, uint32_t tag, uint8_t status, size_t len) {
    (void)tag;
    (void)len;
    msc_csw_buf_t csw;
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    memset(&csw, 0, sizeof(msc_csw_buf_t));
    csw.signature = SCSI_CSW_SIGNATURE;
    csw.tag = tag;
    csw.data_left = 0;
    csw.status = status;
    memcpy(msc->ep_in->ep->buf, &csw, sizeof(msc_csw_buf_t));
    usbdev_ep_ready(msc->ep_in->ep, sizeof(msc_csw_buf_t));
    return 0;
}

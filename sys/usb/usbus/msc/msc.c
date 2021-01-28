/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usb_msc Mass storage
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
#include "usb/descriptor.h"
#include "usb/usbus.h"
#include "usb/usbus/control.h"

#include "usb/msc.h"
#include "usb/usbus/msc/scsi.h"
#include "board.h"

#include <string.h>

#define ENABLE_DEBUG    (1)
#include "debug.h"

static void _event_handler(usbus_t *usbus, usbus_handler_t *handler,
                          usbus_event_usb_t event);
static int _setup_handler(usbus_t *usbus, usbus_handler_t *handler,
                          usbus_setuprq_state_t state, usb_setup_t *setup);
static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                              usbdev_ep_t *ep, usbus_event_transfer_t event);
static void _init(usbus_t *usbus, usbus_handler_t *handler);

static const usbus_handler_driver_t msc_driver = {
    .init = _init,
    .event_handler = _event_handler,
    .transfer_handler = _transfer_handler,
    .setup_handler = _setup_handler,
};

static usbus_msc_device_t msc_handler;

static size_t _gen_msc_descriptor(usbus_t *usbus, void *arg)
{
    (void)arg;
    usb_desc_msc_t msc;
    /* functional msc descriptor */
    msc.length = sizeof(usb_desc_msc_t);
    msc.type = USB_TYPE_DESCRIPTOR_MSC;
    msc.subtype = 0x00;
    usbus_control_slicer_put_bytes(usbus, (uint8_t*)&msc, sizeof(msc));
    return sizeof(usb_desc_msc_t);
}

static const usbus_hdr_gen_funcs_t _msc_descriptor = {
    .get_header = _gen_msc_descriptor,
    .len = {
            .fixed_len = sizeof(usb_desc_msc_t),
    },
    .len_type = USBUS_HDR_LEN_FIXED,
};

int mass_storage_init(usbus_t *usbus)
{
    printf("mass storage init:%p",&msc_handler);
    memset(&msc_handler, 0, sizeof(usbus_msc_device_t));
    msc_handler.usbus = usbus;
    msc_handler.handler_ctrl.driver = &msc_driver;
    usbus_register_event_handler(usbus, ((usbus_handler_t*)(&msc_handler)));
    return 0;
}

static void _init(usbus_t *usbus, usbus_handler_t *handler)
{
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;

    msc->msc_hdr.next = NULL;
    msc->msc_hdr.funcs = &_msc_descriptor;
    msc->msc_hdr.arg = msc;
    printf("msc_hdr:%p\n",&(msc->msc_hdr));
    /* Instantiate interfaces */
    memset(&msc->iface, 0, sizeof(usbus_interface_t));
    /* Configure Interface 0 as control interface */
    msc->iface.class = USB_CLASS_MASS_STORAGE;
    msc->iface.subclass = USB_MSC_SUBCLASS_SCSI_TCS;
    msc->iface.protocol = USB_MSC_PROTOCOL_BULK_ONLY;
    msc->iface.hdr_gen = &(msc->msc_hdr);
    msc->iface.handler = handler;

    /* Create required endpoints */
    usbus_add_endpoint(usbus, &msc->iface, USB_EP_TYPE_BULK, USB_EP_DIR_IN, 64);
    msc->ep_in->interval = 20;
    usbus_add_endpoint(usbus, &msc->iface, USB_EP_TYPE_BULK, USB_EP_DIR_OUT, 64);
    msc->ep_out->interval = 20;

    /* Add interfaces to the stack */
    usbus_add_interface(usbus, &msc->iface);

    usbdev_ep_ready(msc->ep_in->ep, 0);
    usbdev_ep_ready(msc->ep_out->ep, 0);

    usbus_enable_endpoint(msc->ep_in);
    usbus_enable_endpoint(msc->ep_out);
    return;
}

static int _setup_handler(usbus_t *usbus, usbus_handler_t *handler,
                          usbus_setuprq_state_t state, usb_setup_t *setup)
{
     (void)usbus;
      (void)handler;
       (void)state;
        (void)setup;
    #if 0
    (void)usbus;
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;

    DEBUG("ReqSetup:0x%x\n", pkt->request);
    switch(pkt->request) {
        case USB_SETUP_REQ_GET_MAX_LUN:
            static const usbopt_enable_t enable = USBOPT_ENABLE;
            /* Stall as we don't support this feature */
            usbdev_ep_set(msc->ep_in.ep, USBOPT_EP_STALL, &enable, sizeof(usbopt_enable_t));
            return 0;
        case USB_SETUP_REQ_RESET:
            DEBUG("TODO: implement reset setup request\n");
            break;
        default:
            DEBUG("default handle setup rqt:0x%x\n", pkt->request);
            return -1;
    }
    #endif
    return 0;
}

static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                              usbdev_ep_t *ep, usbus_event_transfer_t event)
{
     (void)usbus;
      (void)handler;
       (void)ep;
       (void)event;
    #if 0
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    (void)usbus;

    if (ep == msc->ep_out.ep) {
        size_t len;
        /* Retrieve incoming data */
        usbdev_ep_get(ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));
        if (len > 0) {
            /* Process incoming endpoint buffer */
            scsi_process_cmd(usbus, handler, ep, len);
        }
        return 0;
    }
    else if (ep == msc->ep_in.ep) {
     //   size_t len;
        /* Retrieve incoming data */
      //  usbdev_ep_get(ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));
        //if (len > 0) {
            /* Process incoming endpoint buffer */
          //  scsi_process_in_cmd(usbus, handler, ep, len);
      //  }
        //usbdev_ep_ready(ep, 0);
    }
    #endif
}

static void _event_handler(usbus_t *usbus, usbus_handler_t *handler,
                          usbus_event_usb_t event)
{
     (void)usbus;
      (void)handler;
       (void)event;
    #if 0
    switch(event) {
        case USBUS_MSG_TYPE_SETUP_RQ:
            return _handle_setup(usbus, handler, (usb_setup_t*)arg);
        case USBUS_MSG_TYPE_TR_COMPLETE:
            return _handle_tr_complete(usbus, handler, (usbdev_ep_t*)arg);
        default:
            puts("Unhandled event :0x%x\n");
            return -1;
    }
    #endif
}

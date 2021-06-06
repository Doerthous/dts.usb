/*
   The MIT License (MIT)

   Copyright (c) 2021 Doerthous

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.

   Authour: Doerthous <doerthous@gmail.com>
*/

#include <dts/usb.h>
#include <dts/usb_dev.h>
#include <dts/usb/control_transfer.h>
#include <dts/usb/std_dev_req.h>

//## Internal function
static int usb_dev_req_unpack(usb_dev_req_t *req) 
{
    uint8_t *ptr = req->raw_data;

    if (req->raw_data_size < 8) {
        return 0;
    }
    
    req->bmRequestType = ptr[0];
    req->bRequest = ptr[1];
    req->wValue = ((uint16_t *)ptr)[1];
    req->wIndex  = ((uint16_t *)ptr)[2];
    req->wLength = ((uint16_t *)ptr)[3];

    return 1;
}
static int transaction_data_process(usb_dev_t *usbd, usb_endpoint_t *ep)
{
    uint8_t *xfer_data = usbd->transfer[ep->number].data;
    uint32_t xfer_size = usbd->transfer[ep->number].size;
    if (xfer_size > 0) {
        size_t do_size;
        if (ep->direction == HOST_IN) {
            do_size = usbd->driver->ep_write(ep, xfer_data, xfer_size);
            usbd->transfer[ep->number].data += do_size;
            usbd->transfer[ep->number].size -= do_size;
        }
        else {
            do_size = usbd->driver->ep_read(ep, xfer_data, xfer_size);
            usbd->transfer[ep->number].data += do_size;
            usbd->transfer[ep->number].size -= do_size;
            if (do_size == 0) { // host data out 0 length
                return 0;
            }
        }
    }
    return usbd->transfer[ep->number].size > 0;
}

//## Control transfer state machine
//### Control transfer states
enum
{
    CT_STATE_WAIT_SETUP, // wait host send setup token

    CT_STATE_WAIT_DATA_IN,
    CT_STATE_WAIT_STATUS_OUT,

    CT_STATE_WAIT_DATA_OUT,
    CT_STATE_WAIT_STATUS_IN,
};

//### Control transfer state machine entry
void control_transfer(usb_dev_t *usbd, usb_endpoint_t *ep)
{
    // Event dispatch
    switch (ep->transaction.type) {
        case USB_SETUP_TRANSACTION: {
            control_transfer_setup1(usbd, ep);
        } break;
        case USB_IN_TRANSACTION: {
            control_transfer_in(usbd, ep);
        } break;
        case USB_OUT_TRANSACTION: {
            control_transfer_out1(usbd, ep);
        } break;
        default: {
        }
    }
}


//### Control transfer actions
void control_transfer_do_status_in(usb_dev_t *usbd, usb_endpoint_t *ep)
{
    ep = usb_dev_get_endpoint(usbd, ep->number, HOST_IN); // IN
    usbd->driver->ep_write(ep, 0, 0);
}
void control_transfer_do_data_in
(
    usb_dev_t *usbd, 
    usb_endpoint_t *ep,
    uint8_t *data, 
    size_t size
)
{
    usbd->transfer[ep->number].data = data;
    usbd->transfer[ep->number].size = size;
    usbd->transfer[ep->number].state = CT_STATE_WAIT_DATA_IN;
}
void control_transfer_do_data_out
(
    usb_dev_t *usbd, 
    usb_endpoint_t *ep,
    uint8_t *buff, 
    size_t size
)
{
    usbd->transfer[ep->number].data = buff;
    usbd->transfer[ep->number].size = size;
    usbd->transfer[ep->number].state = CT_STATE_WAIT_DATA_OUT;
}
//### Control transfer events
void control_transfer_in(usb_dev_t *usbd, usb_endpoint_t *ep)
{
    usb_transfer_t *xfer;
    ep = usb_dev_get_endpoint(usbd, ep->number, HOST_IN);
    xfer = &usbd->transfer[ep->number];

    switch (xfer->state) {
        // Host in
        case CT_STATE_WAIT_DATA_IN: {
            if (!transaction_data_process(usbd, ep)) {
                xfer->state = CT_STATE_WAIT_STATUS_OUT;
            }
        } break;
        case CT_STATE_WAIT_STATUS_IN: {
            xfer->state = CT_STATE_WAIT_SETUP;
            ctrl_xfer_callback_t callback;
            callback = (ctrl_xfer_callback_t)xfer->priv;
            if (callback) {
                xfer->priv = NULL;
                callback(usbd, ep);
            }
        } break;
    }
}
void control_transfer_out1(usb_dev_t *usbd, usb_endpoint_t *ep)
{
    usb_transfer_t *xfer;
    ep = usb_dev_get_endpoint(usbd, ep->number, HOST_OUT);
    xfer = &usbd->transfer[ep->number];

    switch (xfer->state) {
        // Host out
        case CT_STATE_WAIT_DATA_OUT: {
            if (!transaction_data_process(usbd, ep)) {
                xfer->state = CT_STATE_WAIT_STATUS_IN;

                /*
                    fake event, in order to prepare tx data before received in 
                    token.

                      out out out in
                                 ^  ^--- read data out completed
                                 |
                                data_out_completed
                */
                control_transfer_in(usbd, ep); 
            }
        } break;
        case CT_STATE_WAIT_STATUS_OUT: {
            uint8_t buff[16];
            size_t size = 16;
            size = usbd->driver->ep_read(ep, buff, size);
            if (size == 0) {
                xfer->state = CT_STATE_WAIT_SETUP;
                ctrl_xfer_callback_t callback;
                callback = (ctrl_xfer_callback_t)xfer->priv;
                if (callback) {
                    xfer->priv = NULL;
                    callback(usbd, ep);
                }
            }
        } break;
    }
}
void control_transfer_setup1(usb_dev_t *usbd, usb_endpoint_t *ep)
{ // prepare tx data then wait IN token
    switch (usbd->transfer[ep->number].state) {
        case CT_STATE_WAIT_SETUP: {
            usb_dev_req_t dev_req;
            uint8_t buff[32];
            dev_req.raw_data = buff;
            dev_req.raw_data_size = 32;
            dev_req.raw_data_size = usbd->driver->ep_read(ep, 
                dev_req.raw_data, dev_req.raw_data_size);
            if (dev_req.raw_data_size > 0) {
                if (usb_dev_req_unpack(&dev_req)) {
                    do {
                        if (USB_STD_DEV_REQ(&dev_req)) {
                            std_dev_req(usbd, ep, &dev_req);
                            break;
                        }
                        if (USB_CLS_DEV_REQ(&dev_req)) {
                            usbd->class_device_request(usbd, ep, &dev_req);
                            break;
                        }
                    } while (0);
 
                    if (USB_DEV_REQ_D2H(&dev_req)) {
                        /*
                            fake event, in order to prepare tx data before 
                            received in token.

                               in in in in ...
                              ^  ^--- read in event
                              |
                              fake in event
                        */
                        control_transfer_in(usbd, ep);
                    }
                }
            }
            else {
                // No-Data Control Transfer
            }
        } break;
    }
}
void control_transfer_setup2(usb_dev_t *usbd, usb_endpoint_t *ep) 
{ // wait IN token then prepare tx data
}

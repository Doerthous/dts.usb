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

#include <dts/usb/usb.h>
#include <dts/usb/usb_dev.h>
#include <dts/usb/dev/control_transfer.h>

//## Internal function
#define low_byte(word) ((word) & 0xFF)
#define high_byte(word) (((word) & 0xFF00) >> 8)
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
static int set_configuration(usb_dev_t *usbd, int idx)
{
    // parser configuration
    uint8_t *conf;
    size_t conf_size;
    
    for (int i = 0; i < usbd->conf_descriptor_count; ++i) {
        conf = usbd->conf_descriptor[i].data;
        conf_size = usbd->conf_descriptor[i].size;
        #define CONFIG_VALUE_OFFSET 5
        if (conf[CONFIG_VALUE_OFFSET] == idx) {
            break;
        }
        conf = NULL;
    }
    if (conf == NULL) {
        if (idx == 0) {
            conf = usbd->conf_descriptor[0].data;
            conf_size = usbd->conf_descriptor[0].size;
        }
        else {
            return 0;
        }
    }

    for (int i = 0; i < conf_size;) {
        struct {
            uint8_t len;
            uint8_t desc_type;
            uint8_t num:4; uint8_t :3; uint8_t dir:1;
            uint8_t type:2; uint8_t :6;
            uint16_t max_pkt_sz;
        } * ep_desc = (void *)(conf+i);
        uint8_t type_map[] = {
            CONTROL_ENDPOINT,
            ISOCHRONOUS_ENDPOINT,
            BULK_ENDPOINT,
            INTERRUPT_ENDPOINT,
        };
        if (ep_desc->desc_type == 0x05) { // Endpoint descriptor
            int addr = ENDPOINT_ADDRESS(ep_desc->num, ep_desc->dir);
            usbd->ep[addr].direction = ep_desc->dir;
            usbd->ep[addr].number = ep_desc->num;
            usbd->ep[addr].type = type_map[ep_desc->type];
            usbd->ep[addr].max_pkt_data_size = ep_desc->max_pkt_sz;
            usbd->ep[addr].enable = 1;
        }
        i += ep_desc->len;
    }
    
    usbd->driver->ep_init(usbd);

    return 1;
}

//## Standard descriptor
enum std_desc_type
{
    DEVICE = 1,
    CONFIGURATION = 2,
    STRING = 3,
    INTERFACE = 4,
    ENDPOINT = 5,
    DEVICE_QUALIFIER = 6,
    OTHER_SPEED_CONFIGURATION = 7,
    INTERFACE_POWER1 = 8,
};

//## Standard device request
//### bRequest
enum std_req_code
{
    GET_STATUS = 0,
    CLEAR_FEATURE = 1,
    SET_FEATURE = 3,
    SET_ADDRESS = 5,
    GET_DESCRIPTOR = 6,
    SET_DESCRIPTOR = 7,
    GET_CONFIGURATION = 8,
    SET_CONFIGURATION = 9,
    GET_INTERFACE = 10,
    SET_INTERFACE = 11,
};

//### handle
void standard_device_request
(
    usb_dev_t *usbd, 
    usb_endpoint_t *ep, 
    usb_dev_req_t *req
)
{
    switch (req->bRequest) {
        case GET_STATUS: {
        } break;
        case CLEAR_FEATURE: {
        } break;
        case SET_FEATURE: {
        } break;
        case SET_ADDRESS: {
            control_transfer_do_status_in(usbd, ep);
            usbd->driver->set_address(usbd->driver, 
                req->wValue);
        } break;
        case GET_DESCRIPTOR: {
            uint8_t type = high_byte(req->wValue);
            uint8_t index = low_byte(req->wValue);
            uint8_t langid = req->wIndex;
            switch (type) {
                case DEVICE: {
                    control_transfer_do_data_in(usbd, ep,
                        usbd->device_descriptor.data,
                        usbd->device_descriptor.size);
                } break;
                case CONFIGURATION: {
                    if (usbd->conf_descriptor[index].size < req->wLength) {
                        req->wLength = usbd->conf_descriptor[index].size;
                    }
                    control_transfer_do_data_in(usbd, ep,
                        usbd->conf_descriptor[index].data,
                        req->wLength);
                } break;
                case STRING: {
                    control_transfer_do_data_in(usbd, ep,
                        usbd->string_descriptor[index].data,
                        usbd->string_descriptor[index].size);
                } break;
                default: {
                    uint8_t fake[512];
                    ep = usb_dev_get_endpoint(usbd, ep->number, HOST_IN); // IN
                    usbd->driver->ep_write(ep, fake, req->wLength);
                } break;
            }
        } break;
        case SET_DESCRIPTOR: {
        } break;
        case GET_CONFIGURATION: {
        } break;
        case SET_CONFIGURATION: {
            uint8_t index = low_byte(req->wValue);
            if (set_configuration(usbd, index)) {
                usbd->set_configuration(usbd, index);
                control_transfer_do_status_in(usbd, ep);
            }
        } break;
        case GET_INTERFACE: {
        } break;
        case SET_INTERFACE: {
        } break;
        default: {
            // Not support
            return;
        }
    }
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
    uint8_t z = 0;
    ep = usb_dev_get_endpoint(usbd, ep->number, HOST_IN); // IN
    usbd->driver->ep_write(ep, &z, 0);
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
    ep = usb_dev_get_endpoint(usbd, ep->number, HOST_IN); // IN
    switch (usbd->transfer[ep->number].state) {
        // Host in
        case CT_STATE_WAIT_DATA_IN: {
            if (!transaction_data_process(usbd, ep)) {
                usbd->transfer[ep->number].state = CT_STATE_WAIT_STATUS_OUT;
            }
        } break;
        case CT_STATE_WAIT_STATUS_IN: {
            usbd->transfer[ep->number].state = CT_STATE_WAIT_SETUP;
            void (*data_out_completed)(usb_dev_t *usbd, usb_endpoint_t *ep);
            data_out_completed = usbd->data_out_completed;
            usbd->data_out_completed = NULL;
            if (data_out_completed) {
                data_out_completed(usbd, ep);
            }
        } break;
    }
}
void control_transfer_out1(usb_dev_t *usbd, usb_endpoint_t *ep)
{
    ep = usb_dev_get_endpoint(usbd, ep->number, HOST_OUT); // IN
    switch (usbd->transfer[ep->number].state) {
        // Host out
        case CT_STATE_WAIT_DATA_OUT: {
            if (!transaction_data_process(usbd, ep)) {
                usbd->transfer[ep->number].state = CT_STATE_WAIT_STATUS_IN;

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
                usbd->transfer[ep->number].state = CT_STATE_WAIT_SETUP;
                void (*data_in_completed)(usb_dev_t *usbd, usb_endpoint_t *ep);
                data_in_completed = usbd->data_in_completed;
                usbd->data_in_completed = NULL;
                if (data_in_completed) {
                    data_in_completed(usbd, ep);
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
                            standard_device_request(usbd, ep, &dev_req);
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

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

#ifndef DTS_USB_DEV_H_
#define DTS_USB_DEV_H_

#include <dts_usb.h>

struct dts_usb_dev;
typedef struct {
    void *driver;
    int (*ep_init)(struct dts_usb_dev *usbd);
    size_t (*ep_read)(dts_usb_endpoint_t *ep, uint8_t *buff, size_t size);
    size_t (*ep_write)(dts_usb_endpoint_t *ep, uint8_t *data, size_t size);
    void (*set_address)(void *driver, uint8_t address);
} dts_usb_dev_drv_t;

typedef struct dts_usb_dev {
    uint8_t address;
    dts_usb_dev_drv_t *driver;
    //int state;
    void (*endpoint_callback[16])(struct dts_usb_dev *usbd, dts_usb_endpoint_t *ep);
    dts_usb_endpoint_t ep[32];

    void (*data_in_completed)(struct dts_usb_dev *usbd, dts_usb_endpoint_t *ep);
    void (*data_out_completed)(struct dts_usb_dev *usbd, dts_usb_endpoint_t *ep);
    dts_usb_transfer_t transfer[16];

    void (*md_init)(struct dts_usb_dev *usbd);

    dts_usb_descriptor_t device_descriptor;
    dts_usb_descriptor_t *conf_descriptor;
    uint8_t conf_descriptor_count;
    dts_usb_descriptor_t *string_descriptor;
    uint8_t string_descriptor_count;

    void (*class_device_request)(struct dts_usb_dev *usbd, 
        dts_usb_endpoint_t *ep, dts_usb_dev_req_t *req);
    void (*set_configuration)(struct dts_usb_dev *usbd, int conf);
} dts_usb_dev_t;

#define ENDPOINT_ADDRESS(number, in) (((number)<<1)+!!in)
static inline dts_usb_endpoint_t *dts_usb_dev_get_endpoint
(
    dts_usb_dev_t *usbd, 
    uint8_t ep_num, 
    int in_dir
)
{
    return (&(usbd->ep[ENDPOINT_ADDRESS(ep_num, in_dir)]));
}

// for low level
static inline dts_usb_endpoint_t *dts_usb_dev_get_first_endpoint
(
    dts_usb_dev_t *usbd
)
{
    return &usbd->ep[0];
}
static inline dts_usb_endpoint_t *dts_usb_dev_get_next_endpoint
(
    dts_usb_dev_t *usbd, 
    dts_usb_endpoint_t *curr
)
{
    int addr = ENDPOINT_ADDRESS(curr->number, curr->direction)+1;
    while (!usbd->ep[addr].enable) {
        if (++addr == 32) {
            return NULL;
        }
    }
    return &usbd->ep[addr];
}

void dts_usb_dev_init(dts_usb_dev_t *usbd);

// event
void dts_usb_dev_reset(dts_usb_dev_t *usbd);
void dts_usb_dev_transaction_received
(
    dts_usb_dev_t *usbd, 
    dts_usb_endpoint_t *ep,
    dts_usb_transaction_t *tra
);

#endif // DTS_USB_DEV_H_

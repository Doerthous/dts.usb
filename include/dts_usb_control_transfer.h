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

#ifndef DTS_USB_CONTROL_TRANSFER_H_
#define DTS_USB_CONTROL_TRANSFER_H_

#include <dts_usb.h>
#include <dts_usb_dev.h>

void dts_usb_control_transfer
(
    dts_usb_dev_t *usbd, 
    dts_usb_endpoint_t *ep
);

typedef void (*dts_usb_ctrl_xfer_callback_t)
(
    dts_usb_dev_t *, 
    dts_usb_endpoint_t *
);

// Actions
void dts_usb_control_transfer_do_status_in
(
    dts_usb_dev_t *usbd,
    dts_usb_endpoint_t *ep
);
void dts_usb_control_transfer_do_data_in
(
    dts_usb_dev_t *usbd,
    dts_usb_endpoint_t *ep,
    uint8_t *data,
    size_t size
);
void dts_usb_control_transfer_do_data_out
(
    dts_usb_dev_t *usbd, 
    dts_usb_endpoint_t *ep,
    uint8_t *buff, 
    size_t size
);

// Events
void dts_usb_control_transfer_in
(
    dts_usb_dev_t *usbd, 
    dts_usb_endpoint_t *ep
);
void dts_usb_control_transfer_out1
(
    dts_usb_dev_t *usbd, 
    dts_usb_endpoint_t *ep
);
void dts_usb_control_transfer_setup1
(
    dts_usb_dev_t *usbd, 
    dts_usb_endpoint_t *ep
);

#endif // DTS_USB_CONTROL_TRANSFER_H_

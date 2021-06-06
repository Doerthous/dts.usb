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

#ifndef DTS_USB_BULK_TRANSFER_H_
#define DTS_USB_BULK_TRANSFER_H_

#include <dts_usb.h>
#include <dts_usb_dev.h>

void dts_usb_bulk_transfer(dts_usb_dev_t *usb, dts_usb_endpoint_t *ep);

size_t dts_usb_bulk_write
(
    dts_usb_dev_t *usb, 
    int ep, 
    uint8_t *data, 
    size_t size
);

size_t dts_usb_bulk_read
(
    dts_usb_dev_t *usb, 
    int ep, 
    uint8_t *data, 
    size_t size
);



typedef void (*dts_usb_bulk_callback_t)(dts_usb_dev_t *usb, int ep);
// call this when set_configuration to set the given ep as a bulk in endpoint
void dts_usb_bulk_set_in_endpoint
(
    dts_usb_dev_t *usb, 
    int ep, 
    dts_usb_bulk_callback_t callback
);
// call this when set_configuration to set the given ep as a bulk out endpoint
void dts_usb_bulk_set_out_endpoint
(
    dts_usb_dev_t *usb, 
    int ep, 
    dts_usb_bulk_callback_t notifier
);

#endif // DTS_USB_BULK_TRANSFER_H_

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

#ifndef USB_DEV_H_
#define USB_DEV_H_

#include <dts_usb_dev.h>

#define usb_dev_drv_t dts_usb_dev_drv_t

#define usb_dev_t dts_usb_dev_t
#define usb_dev_get_endpoint dts_usb_dev_get_endpoint
#define usb_dev_get_first_endpoint dts_usb_dev_get_first_endpoint
#define usb_dev_get_next_endpoint dts_usb_dev_get_next_endpoint
#define usb_dev_init dts_usb_dev_init
#define usb_dev_reset dts_usb_dev_reset
#define usb_dev_transaction_received dts_usb_dev_transaction_received

#endif // USB_DEV_H_

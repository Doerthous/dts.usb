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

#ifndef DTS_USB_COM_DEV_H_
#define DTS_USB_COM_DEV_H_

#include <dts_usb_dev.h>
#include <dts_usb_com_dev_line_coding.h>
#include <dts_usb_misc.h>

typedef struct
{
    dts_usb_dev_t dev;
    union {
        dts_usb_com_dev_line_coding_t line_coding;
    } u;

    union {
        #define bit(x) (1<<(x))
        #define PSTN_EVENT_LINE_CODING_CHANGED bit(0)
        dts_usb_event_t pstn;
    } event;
} dts_usb_com_dev_t;

void dts_usb_com_dev_received
(
    dts_usb_dev_t *dev, 
    dts_usb_endpoint_t *ep, 
    dts_usb_dev_req_t *req
);

#endif // DTS_USB_COM_DEV_H_

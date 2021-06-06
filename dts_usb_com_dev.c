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
#include <dts/usb_com_dev.h>
#include <dts/usb/com_dev/line_coding.h>
#include <dts/usb/misc.h>

#define to_com_dev(usb) ((usb_com_dev_t *)usb)

enum cdc_req_code
{
    SET_LINE_CODING = 0x20, // [USBPSTN1.2]
    GET_LINE_CODING = 0x21, // [USBPSTN1.2]
    SET_CONTROL_LINE_STATE = 0x22, // [USBPSTN1.2]
    SET_OPERATION_PARMS = 0x32, // [USBPSTN1.2]
    GET_OPERATION_PARMS = 0x33, // [USBPSTN1.2]
    SET_LINE_PARMS = 0x34, // [USBPSTN1.2]
    GET_LINE_PARMS = 0x35, // [USBPSTN1.2]
};


static void line_coding_received(usb_dev_t *usbd, usb_endpoint_t *ep)
{
    usb_com_dev_t *usbcd = (usb_com_dev_t *)usbd;

    if (line_coding_unpack(&usbcd->u.line_coding)) {
        event_set(&usbcd->event.pstn, PSTN_EVENT_LINE_CODING_CHANGED);
    }
    ctrl_xfer_ack(usbd, ep);
}
void usb_com_dev_received
(
    usb_dev_t *usbd,
    usb_endpoint_t *ep,
    usb_dev_req_t *req
)
{
    usb_com_dev_t *usbcd = to_com_dev(usbd);

    switch (req->bRequest) {
        case SET_LINE_CODING: {
            ctrl_xfer_set_read_callback(usbd, ep, line_coding_received);
            ctrl_xfer_read(usbd, ep, 
                usbcd->u.line_coding.raw_data, req->wLength);
        } break;
        case GET_LINE_CODING: {
            #define LINE_CODING_SIZE 7
            line_coding_pack(&usbcd->u.line_coding);
            ctrl_xfer_write(usbd, ep, 
                usbcd->u.line_coding.raw_data, LINE_CODING_SIZE);
        } break;
        case SET_CONTROL_LINE_STATE: {
            ctrl_xfer_ack(usbd, ep);
        } break;
        case SET_OPERATION_PARMS: {
        } break;
        case GET_OPERATION_PARMS: {
        } break;
        case SET_LINE_PARMS: {
        } break;
        case GET_LINE_PARMS: {
        } break;
    }
}

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
#include <dts/usb/usb_com_dev.h>
#include <dts/usb/com_dev/line_coding.h>

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

static uint8_t data_out[32];
static size_t data_out_size;
static void data_out_completed(usb_dev_t *usbd, usb_endpoint_t *ep)
{
    control_transfer_do_status_in(usbd, ep);
}
void usb_com_dev_received
(
    usb_dev_t *usbd,
    usb_endpoint_t *ep,
    usb_dev_req_t *req
)
{
    usb_com_dev_t *usbcd = (usb_com_dev_t *)usbd;

    switch (req->bRequest) {
        case SET_LINE_CODING: {
            data_out_size = req->wLength;
            control_transfer_do_data_out(usbd, ep, data_out, req->wLength);
            usbd->data_out_completed = data_out_completed;
        } break;
        case GET_LINE_CODING: {
            control_transfer_do_data_in(usbd, ep, (uint8_t*)usbcd->line_coding, 
                sizeof(line_coding_t));
        } break;
        case SET_CONTROL_LINE_STATE: {
            control_transfer_do_status_in(usbd, ep);
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

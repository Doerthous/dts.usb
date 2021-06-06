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

#ifndef DTS_USB_H_
#define DTS_USB_H_

#include <stdint.h>
#include <stddef.h>

typedef struct 
{
    #define USB_SETUP_TRANSACTION 1
    #define USB_OUT_TRANSACTION 2
    #define USB_IN_TRANSACTION 3
    uint8_t type; // Setup, Out, In, ...
} dts_usb_transaction_t;

typedef struct 
{ 
    uint8_t state; 
    uint8_t *data; 
    size_t size;

    // 1. use as high level callback in bulk transfer
    void *priv;
} dts_usb_transfer_t;

typedef struct
{
    #define HOST_OUT 0
    #define HOST_IN 1
    uint8_t direction:1; // current data flow direction
    #define BULK_ENDPOINT 0
    #define CONTROL_ENDPOINT 1
    #define INTERRUPT_ENDPOINT 2
    #define ISOCHRONOUS_ENDPOINT 3
    uint8_t type:2;
    uint8_t number:4;
    // 
    size_t max_pkt_data_size:31;
    size_t enable:1;

    //
    dts_usb_transaction_t transaction;
} dts_usb_endpoint_t;

typedef struct 
{
    uint8_t *data;
    size_t size;
} dts_usb_descriptor_t;

typedef struct {
    #define USB_DEV_REQ_D2H(req) ((req)->bmRequestType & (0x80))
    #define USB_STD_DEV_REQ(req) !((req)->bmRequestType & (0x03 << 5))
    #define USB_CLS_DEV_REQ(req) ((req)->bmRequestType & (0x01 << 5))
    #define USB_VND_DEV_REQ(req) ((req)->bmRequestType & (0x02 << 5))
    uint8_t bmRequestType;

    uint8_t bRequest;

    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;

    uint8_t *raw_data;
    uint16_t raw_data_size;
} dts_usb_dev_req_t;


#endif // DTS_USB_H_

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

// event
#include <string.h>
void usb_dev_reset(usb_dev_t *usbd) 
{
    usbd->driver->set_address(usbd->driver, 0);

    memset(usbd->ep, 0, sizeof(usbd->ep));
    int addr = ENDPOINT_ADDRESS(0, HOST_OUT);
    usbd->ep[addr].direction = HOST_OUT;
    usbd->ep[addr].number = 0;
    usbd->ep[addr].type = CONTROL_ENDPOINT;
    usbd->ep[addr].max_pkt_data_size = 64;
    usbd->ep[addr].enable = 1;
    addr = ENDPOINT_ADDRESS(0, HOST_IN);
    usbd->ep[addr].direction = HOST_IN;
    usbd->ep[addr].number = 0;
    usbd->ep[addr].type = CONTROL_ENDPOINT;
    usbd->ep[addr].max_pkt_data_size = 64;
    usbd->ep[addr].enable = 1;
    usbd->endpoint_callback[0] = control_transfer;

    usbd->driver->ep_init(usbd);
}

void usb_dev_transaction_received
(
    usb_dev_t *usbd, 
    usb_endpoint_t *ep, 
    usb_transaction_t *tra
)
{
    ep->transaction = *tra;
    usbd->endpoint_callback[ep->number](usbd, ep);
}

void usb_dev_init(usb_dev_t *usbd)
{
    usbd->md_init(usbd);
}

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

#include <dts/usb/bulk_transfer.h>

#if 0
//## Control transfer state machine
//### Control transfer states
enum
{
    BT_STATE_IDLE,
    BT_STATE_XFER,
};
static void transaction_data_process(usb_dev_t *usb, usb_endpoint_t *ep)
{
    usb_transfer_t *xfer = &usb->transfer[ep->number];
    if (xfer->size > 0) {
        size_t do_size;
        if (ep->direction == HOST_IN) {
            do_size = usb->driver->ep_write(ep, xfer->data, xfer->size);
            xfer->data += do_size;
            xfer->size -= do_size;
        }
    }
    else if (xfer->state != BT_STATE_IDLE) {
        xfer->state = BT_STATE_IDLE;
        if (ep->direction == HOST_IN) {
            if (xfer->priv) {
                ((bulk_callback_t)(xfer->priv))(usb, ep->number);
            }

            // xfer->state may not be BT_STATE_IDLE because callback initiate 
            // another transfer.
            if (xfer->state == BT_STATE_IDLE) {
                /**
                 * When using CDC, device to host (IN) need a zero-length packet
                 * to end the transfer.
                 */ 
                usb->driver->ep_write(ep, 0, 0);
            }
        }
    }
}
#endif 

size_t bulk_write(usb_dev_t *usb, int _ep, uint8_t *data, size_t size)
{
    usb_endpoint_t *ep = usb_dev_get_endpoint(usb, _ep, HOST_IN); // IN

    return usb->driver->ep_write(ep, data, size);

    #if 0
    if (xfer->state == BT_STATE_XFER) {
        return 0;
    }
    xfer->state = BT_STATE_XFER;

    xfer->data = data;
    xfer->size = size;

    transaction_data_process(usb, ep);

	return 1;
    #endif
}

size_t bulk_read(usb_dev_t *usb, int _ep, uint8_t *data, size_t size)
{
    usb_endpoint_t *ep = usb_dev_get_endpoint(usb, _ep, HOST_OUT);

	return usb->driver->ep_read(ep, data, size);
}


void bulk_transfer(usb_dev_t *usb, usb_endpoint_t *ep)
{
    usb_transfer_t *xfer = &usb->transfer[ep->number];

    switch (ep->transaction.type) {
        case USB_IN_TRANSACTION: {
            //transaction_data_process(usb, ep);
            if (xfer->priv) {
                ((bulk_callback_t)(xfer->priv))(usb, ep->number);
            }
        } break;
        case USB_OUT_TRANSACTION: {
            if (xfer->priv) {
                ((bulk_callback_t)(xfer->priv))(usb, ep->number);
            }
        } break;
        default: {
        }
    }
}


void bulk_set_in_endpoint(usb_dev_t *usb, int ep, bulk_callback_t callback)
{
    usb->endpoint_callback[ep] = bulk_transfer;
    usb->transfer[ep].priv = callback;
}

void bulk_set_out_endpoint(usb_dev_t *usb, int ep, bulk_callback_t notifier)
{
    usb->endpoint_callback[ep] = bulk_transfer;
    usb->transfer[ep].priv = notifier;
}

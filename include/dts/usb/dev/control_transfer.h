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

#ifndef CONTROL_TRANSFER_H_
#define CONTROL_TRANSFER_H_

// Maybe someday we will put control_transfer into dts.usb module other than
// dts.usb.dev
#include <dts_usb_dev_control_transfer.h>

#define control_transfer dts_usb_dev_control_transfer

#define control_transfer_do_status_in dts_usb_dev_control_transfer_do_status_in
#define control_transfer_do_data_in dts_usb_dev_control_transfer_do_data_in
#define control_transfer_do_data_out dts_usb_dev_control_transfer_do_data_out
#define control_transfer_in dts_usb_dev_control_transfer_in
#define control_transfer_out1 dts_usb_dev_control_transfer_out1
#define control_transfer_setup1 dts_usb_dev_control_transfer_setup1
#define control_transfer_setup2 dts_usb_dev_control_transfer_setup2

#endif // CONTROL_TRANSFER_H_

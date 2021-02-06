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

#ifndef DTS_USB_COM_DEV_LINE_CODING_H_
#define DTS_USB_COM_DEV_LINE_CODING_H_

#include <stdint.h>

typedef struct 
{
    /* Data terminal rate, in bits per second
    */
    uint32_t dwDTERate;

    /* Stop bits
        0 - 1 Stop bit
        1 - 1.5 Stop bits
        2 - 2 Stop bits
    */
    uint8_t bCharFormat;

    /* Parity
        0 - None
        1 - Odd
        2 - Even
        3 - Mark
        4 - Space
    */
    uint8_t bParityType;

    /* Data bits (5, 6, 7, 8 or 16)
    */
    uint8_t bDataBits; 
} dts_usb_com_dev_line_coding_t;

#endif // DTS_USB_COM_DEV_LINE_CODING_H_

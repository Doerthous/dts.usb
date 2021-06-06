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

#include <dts/usb/com_dev/line_coding.h>

#define usb_to_cpu_u32(buff, pu32) do \
{ \
    *(pu32) = (((buff)[0]) \
                | ((buff)[1] << 8) \
                | ((buff)[2]<<16) \
                | ((buff)[3]<<24)); \
} while (0)

#define cpu_to_usb_u32(u32, buff) do \
{ \
    (buff)[0] = (u32); \
    (buff)[1] = (u32)>>8; \
    (buff)[2] = (u32)>>16; \
    (buff)[3] = (u32)>>24; \
} while (0)

int line_coding_unpack(line_coding_t *coding)
{
    usb_to_cpu_u32(coding->raw_data, &coding->dwDTERate);
    coding->bCharFormat = coding->raw_data[4];
    coding->bParityType = coding->raw_data[5];
    coding->bDataBits = coding->raw_data[6];

    return 1;
}

int line_coding_pack(line_coding_t *coding)
{
    cpu_to_usb_u32(coding->dwDTERate, coding->raw_data);
    coding->raw_data[4] = coding->bCharFormat;
    coding->raw_data[5] = coding->bParityType;
    coding->raw_data[6] = coding->bDataBits;

    return 1;
}

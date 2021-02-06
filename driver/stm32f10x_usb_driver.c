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

#include <usb_lib.h> // require only usb_regs.c and usb_mem.c
#include <dts/usb/usb.h>
#include <dts/usb/usb_dev.h>

/* How to use:

    To use this driver, you need to provide STM32 USB-FS-Device Driver V4.0.0.
    
    1. Use usb_regs.c and usb_mem.c only
    2. provide hw_confg.h which include the stm32f10x header file:
        #include <stm32f10x.h>
    3. provide usb_conf.h with nothing.
*/

// Driver
static int ep_init(usb_dev_t *usbd)
{
    uint16_t type_map[4] = {
        [BULK_ENDPOINT] = EP_BULK, 
        [CONTROL_ENDPOINT] = EP_CONTROL, 
        [INTERRUPT_ENDPOINT] = EP_INTERRUPT, 
        [ISOCHRONOUS_ENDPOINT] = EP_ISOCHRONOUS
    };
    
    #define BTABLE_ADDRESS (0x00)
    SetBTABLE(BTABLE_ADDRESS);
    
    for (int i = 0; i < 7; ++i) {
        SetEPTxStatus(i, EP_TX_DIS);
        SetEPRxStatus(i, EP_RX_DIS);
    }
    
    size_t offset = 64;
    usb_endpoint_t *ep = usb_dev_get_first_endpoint(usbd);
    while (ep) {
        if (ep->direction == HOST_IN) {
            SetEPType(ep->number, type_map[ep->type]);
            SetEPTxCount(ep->number, 0);
            SetEPTxAddr(ep->number, offset);
            SetEPTxStatus(ep->number, EP_TX_NAK);
        }
        else {
            SetEPType(ep->number, type_map[ep->type]);
            SetEPRxCount(ep->number, ep->max_pkt_data_size);
            SetEPRxAddr(ep->number, offset);
            SetEPRxValid(ep->number);
        }
        offset += ep->max_pkt_data_size;
        ep = usb_dev_get_next_endpoint(usbd, ep);
    }

    Clear_Status_Out(ENDP0);

    return 1;
}
static size_t ep_read(usb_endpoint_t *ep, uint8_t *buff, size_t size)
{
    size_t do_size = GetEPRxCount(ep->number);
    if (size < do_size) {
        do_size = size;
    }
    PMAToUserBufferCopy(buff, GetEPRxAddr(ep->number), do_size);
    /* re-enable for next data reception */
    SetEPRxStatus(ep->number, EP_RX_VALID);
    return do_size;
}
static size_t ep_write(usb_endpoint_t *ep, uint8_t *data, size_t size)
{
    size_t do_size = size;

    if (ep->max_pkt_data_size < size) {
        do_size = ep->max_pkt_data_size;
    }

    UserToPMABufferCopy(data, GetEPTxAddr(ep->number), do_size);
    SetEPTxCount(ep->number, do_size);
    SetEPTxStatus(ep->number, EP_TX_VALID);

    return do_size;
}
static void set_address(void *driver, uint8_t address)
{
    int ep_count = 8;

    // set address in every used endpoint
    for (int i = 0; i < ep_count; i++) {
        _SetEPAddress((uint8_t)i, (uint8_t)i);
    }
    
    // set device address and enable function
    _SetDADDR(address | DADDR_EF);
}
static struct drv 
{
    usb_dev_t *dev;
} drv;
static usb_dev_drv_t stm32f10x_usb_drv = 
{
    .driver = &drv,
    .ep_init = ep_init,
    .ep_read = ep_read,
    .ep_write = ep_write,
    .set_address = set_address,
};

// Interrupt ISR
void stm32f10x_usb_isr(void)
{
    uint16_t wIstr, wEPVal;
    uint8_t ep_num;
    int in_dir;
    usb_transaction_t transaction;
    usb_dev_t *usbd = drv.dev;
    
    wIstr = GetISTR();
    while (wIstr & ISTR_CTR) {
        // extract highest priority endpoint number
        ep_num = (uint8_t)(wIstr & ISTR_EP_ID);
        in_dir = !(wIstr & ISTR_DIR);
        if (in_dir) {
            transaction.type = USB_IN_TRANSACTION;
            _ClearEP_CTR_TX(ep_num);
        }
        else {
            wEPVal = GetENDPOINT(ep_num);
            if ((wEPVal & EP_SETUP)) {
                transaction.type = USB_SETUP_TRANSACTION;
            }
            else {
                transaction.type = USB_OUT_TRANSACTION;
            }
            _ClearEP_CTR_RX(ep_num);
        }
        usb_endpoint_t *ep = usb_dev_get_endpoint(usbd, ep_num, in_dir);
        _SetISTR((uint16_t)CLR_CTR);
        usb_dev_transaction_received(usbd, ep, &transaction);

        wIstr = GetISTR();
    }
    
    if (wIstr & ISTR_RESET) {
        _SetISTR((uint16_t)CLR_RESET);

        usb_dev_reset(usbd);
    }
}

// Init
void usb_line_config()
{
    // TODO: maybe someday we need to configure the D+/D- line.
}

static uint16_t w_int_mask;
static int power_on(void)
{
    usb_line_config();

    // Force reset
    _SetCNTR(CNTR_FRES);

    w_int_mask = 0;
    _SetCNTR(w_int_mask);

    // Clear pending interrupts
    _SetISTR(0);

    // Set interrupt mask
    w_int_mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
    _SetCNTR(w_int_mask);

    return 1;
}

void stm32f10x_usb_device_init(usb_dev_t *usbd)
{
    usbd->driver = &stm32f10x_usb_drv;
    ((struct drv  *)stm32f10x_usb_drv.driver)->dev = usbd;

    // USB clock init
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

    // USB interrupt init
    NVIC_InitTypeDef NVIC_InitStructure; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn; // USB wake up
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    power_on();

    _SetISTR(0);
    w_int_mask = (CNTR_CTRM
                    | CNTR_WKUPM
                    | CNTR_SUSPM
                    | CNTR_ERRM
                    | CNTR_SOFM
                    | CNTR_ESOFM 
                    | CNTR_RESETM 
                    );
    _SetCNTR(w_int_mask);
}

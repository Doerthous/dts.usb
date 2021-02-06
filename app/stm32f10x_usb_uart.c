#include "usb_desc.h"

#include <dts/usb/usb.h>
#include <dts/usb/usb_dev.h>
#include <dts/usb/usb_com_dev.h>
#include <dts/usb/com_dev/line_coding.h>

usb_descriptor_t conf_desc =
{
    (uint8_t *)Virtual_Com_Port_ConfigDescriptor,
    VIRTUAL_COM_PORT_SIZ_CONFIG_DESC,
};
usb_descriptor_t string_descriptor[4] =
{
    {(uint8_t*)Virtual_Com_Port_StringLangID, VIRTUAL_COM_PORT_SIZ_STRING_LANGID},
    {(uint8_t*)Virtual_Com_Port_StringVendor, VIRTUAL_COM_PORT_SIZ_STRING_VENDOR},
    {(uint8_t*)Virtual_Com_Port_StringProduct, VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT},
    {(uint8_t*)Virtual_Com_Port_StringSerial, VIRTUAL_COM_PORT_SIZ_STRING_SERIAL}
};

static line_coding_t uart_conf = {
    .dwDTERate = 9600,
    .bCharFormat = 1,
    .bParityType = 0,
    .bDataBits = 8,
};
static void interrupt_in(usb_dev_t *usbd, usb_endpoint_t *ep)
{
	ep->number = ep->number;
}
static void bulk_in(usb_dev_t *usbd, usb_endpoint_t *ep)
{
	ep->number = ep->number;
}
static void bulk_out(usb_dev_t *usbd, usb_endpoint_t *ep)
{
    uint8_t buff[64];
    size_t size;

    // echo back
    size = usbd->driver->ep_read(ep, buff, 64);
    ep = usb_dev_get_endpoint(usbd, 1, HOST_IN); // IN
    size = usbd->driver->ep_write(ep, buff, size);
}
static void set_configuration(usb_dev_t *usbd, int conf)
{
    usbd->endpoint_callback[1] = bulk_in;
    usbd->endpoint_callback[2] = interrupt_in;
    usbd->endpoint_callback[3] = bulk_out;
}

void stm32f10x_usb_device_init(usb_dev_t *usbd);
usb_com_dev_t usb_com_dev = 
{
    .dev.md_init = stm32f10x_usb_device_init,
    .dev.device_descriptor = { 
        (uint8_t *)Virtual_Com_Port_DeviceDescriptor,
        VIRTUAL_COM_PORT_SIZ_DEVICE_DESC,
    },
    .dev.conf_descriptor = &conf_desc,
    .dev.conf_descriptor_count = 1,
    .dev.string_descriptor = string_descriptor,
    .dev.string_descriptor_count = 4,
    .dev.class_device_request = usb_com_dev_received,
    .dev.set_configuration = set_configuration,
    .line_coding = &uart_conf,
};

#include "usb_desc.h"

#include <dts/usb.h>
#include <dts/usb_dev.h>
#include <dts/usb_com_dev.h>
#include <dts/usb/bulk_transfer.h>

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

static void interrupt_in(usb_dev_t *usbd, usb_endpoint_t *ep)
{
	ep->number = ep->number;
}
#if 0
static void read_notifier(usb_dev_t *usb, int ep)
{
    uint8_t buff[64];
    size_t size;

	size = bulk_read(usb, ep, buff, 64);
	bulk_write(usb, 1, buff, 1);
}
static void write_callback(usb_dev_t *usb, int ep)
{
	static uint8_t buff[4096];
	bulk_write(usb, ep, buff, 4096);
}
#endif
static void set_configuration(usb_dev_t *usbd, int conf)
{
	//bulk_set_in_endpoint(usbd, 1, write_callback);
    usbd->endpoint_callback[2] = interrupt_in;
    //bulk_set_out_endpoint(usbd, 3, read_notifier);
	
	bulk_set_in_endpoint(usbd, 1, NULL);
    bulk_set_out_endpoint(usbd, 3, NULL);
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
    .u.line_coding = {
		.dwDTERate = 9600,
		.bCharFormat = 1,
		.bParityType = 0,
		.bDataBits = 8,
	},
};

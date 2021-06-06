#include <stm32f10x.h>
#include <dts/usb_dev.h>
#include <dts/usb_com_dev.h>
#include <dts/usb/com_dev/line_coding.h>
#include <dts/usb/misc.h>
#include <dts/usb/bulk_transfer.h>

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    void stm32f10x_usb_isr(void);
    stm32f10x_usb_isr();
}

void USBWakeUp_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line18);
}


uint8_t cmd;
size_t size;
extern usb_com_dev_t usb_com_dev;
int main(void)
{
    static uint8_t buff[4096];

    usb_dev_init((usb_dev_t *)&usb_com_dev);
    
    while (1) {
		if (event_wait(&usb_com_dev.event.pstn, DTS_USB_EVENT_ALL)) {
			if (event_test(&usb_com_dev.event.pstn, PSTN_EVENT_LINE_CODING_CHANGED)) {
				event_clear(&usb_com_dev.event.pstn, PSTN_EVENT_LINE_CODING_CHANGED);
			}
		}

        switch (cmd) {
            case 1: {
                size = bulk_read((usb_dev_t *)&usb_com_dev, 3, buff, 64);
            } break;
            case 2: {
                size = bulk_write((usb_dev_t *)&usb_com_dev, 1, buff, 64);
            } break;
			default: {
				size = bulk_read((usb_dev_t *)&usb_com_dev, 3, &cmd, 1);
				if (size) {
					++size;
				}
			}
        }
	}
    
    return 0;
}

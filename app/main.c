#include <stm32f10x.h>
#include <dts/usb/usb_dev.h>
#include <dts/usb/usb_com_dev.h>
#include <dts/usb/com_dev/line_coding.h>

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    void stm32f10x_usb_isr(void);
    stm32f10x_usb_isr();
}

void USBWakeUp_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line18);
}

extern usb_com_dev_t usb_com_dev;
int main(void)
{
    usb_dev_init((usb_dev_t *)&usb_com_dev);
    
    while (1);
    
    return 0;
}

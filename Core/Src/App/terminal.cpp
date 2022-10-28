/*
 * terminal.cpp
 *
 *  Created on: Oct 26, 2022
 *      Author: nitin
 */

#include "usb_device.h"
#include "cmsis_os2.h"
#include "usbd_cdc_if.h"
#include <cstdarg>

extern "C"
{

extern UART_HandleTypeDef huart2;
volatile char terminalConnected = 0;

void
terminal_putchar(unsigned char ch)
{
    if (!terminalConnected)
    {
        return;
    }
    while(CDC_Transmit_FS(&ch, 1) == USBD_BUSY);
}

void
terminal_printf(const char* format, ...)
{
    static uint8_t buffer[100];
    va_list va_args;

    va_start(va_args, format);
    size_t len = vsnprintf((char*) buffer, sizeof(buffer), format, va_args);
    va_end(va_args);

    if (!terminalConnected)
    {
        return;
    }
    while(CDC_Transmit_FS(buffer, len) == USBD_BUSY);
}

static uint8_t buffer[200];
void TerminalTask(void *argument)
{
    bool flag = false;
    uint16_t bytesReceived;
    MX_USB_DEVICE_Init();

    for(;;)
    {
        if (HAL_UARTEx_ReceiveToIdle(&huart2, buffer, sizeof(buffer) - 1, &bytesReceived, 1000) == HAL_OK)
        {
            buffer[bytesReceived] = '\0';
            terminal_printf("%s", buffer);
        }

        HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, flag ? GPIO_PIN_SET : GPIO_PIN_RESET);
        flag = !flag;
    }
}

}

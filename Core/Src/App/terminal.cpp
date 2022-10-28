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

static uint8_t buffer[200];
void TerminalTask(void *argument)
{
    bool flag = false;
    uint16_t bytesReceived;
    MX_USB_DEVICE_Init();

    for(;;)
    {
        if (HAL_UARTEx_ReceiveToIdle(&huart2, buffer, sizeof(buffer), &bytesReceived, 1000) == HAL_OK)
        {
            if (!terminalConnected)
            {
                continue;
            }

            while(CDC_Transmit_FS(buffer, bytesReceived) == USBD_BUSY);
        }

        HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, flag ? GPIO_PIN_SET : GPIO_PIN_RESET);
        flag = !flag;
    }
}

}

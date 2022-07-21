#include "hw_config.h"
#include "usbd_def.h"
#include "usbd_ctlreq.h"
#include "usbd_core.h"
#include "sys.h"
#include "usart.h"  
#include "string.h"	
#include "stdarg.h"		 
#include "stdio.h"	
#include "stm32f1xx_hal_gpio.h"
#include "delay.h"
extern USBD_HandleTypeDef hUsbDeviceFS;
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//USB-hw_config 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/28
//修改日期:2019/11/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
	*Error_Handler for usb
    *发生错误时的回调函数，用户可以在其加入调试信息
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
}

/*@brief 上电后拉低USB数据线，让主机识别为断开，
 * 延时一段时间再释放去初始化，避免了复位后需要插拔数据线。
 */
void USB_Reset()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
 
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
 
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	delay_us(700);//delay 10 ms
}
/*
	获取设备状态，返回当前的设备状态
	USBD_STATE_DEFAULT                              0x01U
	USBD_STATE_ADDRESSED                            0x02U
	USBD_STATE_CONFIGURED                           0x03U
	USBD_STATE_SUSPENDED                            0x04U
*/
uint8_t USB_GetStatus()
{
	return hUsbDeviceFS.dev_state;//返回设备状态
}

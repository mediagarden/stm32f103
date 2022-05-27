/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "led.h"
#include "lcd.h"
#include "sram.h"
#include "usart.h"	 
#include "touch.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl/demos/keypad_encoder/lv_demo_keypad_encoder.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验27
 触摸屏实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();
	delay_init(72);               		//初始化延时函数
	uart_init(9600);					//初始化串口
	LED_Init();							//初始化LED	
	KEY_Init();							//初始化按键
	LCD_Init();							//LCD初始化
	SRAM_Init();						//初始化外部SRAM  			
 	tp_dev.init();					    //触摸屏初始化
	//simple_printf("lv_init\r\n");
	lv_init();							//lvgl系统初始化
	lv_port_disp_init();		        //lvgl显示接口初始化
	lv_port_indev_init();		        //lvgl输入接口初始化

	lv_demo_keypad_encoder();
	//simple_printf("while\r\n");
	while(1)
	{
		//simple_printf("lv_task_handler\r\n");
		tp_dev.scan(0);
		lv_task_handler();
	}
}

void LVGL_IncTick(void)
{
	lv_tick_inc(1);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */

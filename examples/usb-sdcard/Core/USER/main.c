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
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "malloc.h"
#include "sdio_sdcard.h"
#include "w25qxx.h"
#include "hw_config.h"
#include "usb_device.h"
#include "usbd_storage_if.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验49
 USB读卡器实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司
 作者：正点原子 @ALIENTEK
************************************************/

extern vu8 USB_STATUS_REG;			//USB状态
extern vu8 bDeviceState;			//USB连接 情况
extern USBD_HandleTypeDef hUsbDeviceFS;
extern SD_HandleTypeDef hsd;     	//SD卡句柄


int main(void)
{
    u8 offline_cnt=0;
    u8 tct=0;
    u8 USB_STA;
    u8 Device_STA;
    HAL_Init();                    	 	//初始化HAL库
    SystemClock_Config();   			//设置时钟,72M
    delay_init(72);               		//初始化延时函数
    uart_init(115200);					//初始化串口
    LED_Init();							//初始化LED
    LCD_Init();           				//初始化LCD FSMC接口
    W25QXX_Init();						//初始化W25Q128
    my_mem_init(SRAMIN);				//初始化内部内存池

    POINT_COLOR=RED;					//设置字体为红色
    LCD_ShowString(30,50,200,16,16,"WarShip STM32");
    LCD_ShowString(30,70,200,16,16,"USB Card Reader TEST");
    LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
    LCD_ShowString(30,110,200,16,16,"2019/10/10");
    if(SD_Init())//SD卡初始化
		LCD_ShowString(30,130,200,16,16,"SD Card Error!");	//检测SD卡错误
    else //SD 卡正常
    {
        LCD_ShowString(30,130,200,16,16,"SD Card Size:     MB");
        SD_GetCardInfo(&SDCardInfo);    //获取SD卡信息
        Mass_Memory_Size[1]=(uint64_t)(SDCardInfo.LogBlockNbr)*(uint64_t)(SDCardInfo.LogBlockSize);	//计算SD卡容量
	    Mass_Block_Size[1] =MSC_MEDIA_PACKET;//因为我们在Init里面设置了SD卡的操作字节为512个,所以这里一定是512个字节. 宏定义名称为MSC_MEDIA_PACKET
	    Mass_Block_Count[1]=Mass_Memory_Size[1]/Mass_Block_Size[1];
        LCD_ShowNum(134,130,Mass_Memory_Size[1]>>20,5,16);	//显示SD卡容量
    }

	if(W25QXX_TYPE!=W25Q128)
        LCD_ShowString(30,150,200,16,16,"W25Q128 Error!");	//检测W25Q128错误
    else //SPI FLASH 正常
    {
		Mass_Memory_Size[0]=1024*1024*12;	//前12M字节
	    Mass_Block_Size[0] =512;			//设置SPI FLASH的操作扇区大小为512
	    Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];
        LCD_ShowString(30,150,200,16,16,"SPI FLASH Size:12MB");
    }
    LCD_ShowString(30,170,200,16,16,"USB Connecting...");//提示USB开始连接
    USB_Reset();
    MX_USB_DEVICE_Init();//usb初始化
    LCD_ShowString(30,190,200,16,16,"USB inited..."); 
	delay_ms(1800);	
    while(1)
    {
        delay_ms(1);
        if(USB_STA!=USB_STATUS_REG)//状态改变了
        {
            LCD_Fill(30,190,240,190+16,WHITE);//清除显示
            if(USB_STATUS_REG&0x01)//正在写
            {
                LCD_ShowString(30,190,200,16,16,"USB Writing...");//提示USB正在写入数据
            }
            if(USB_STATUS_REG&0x02)//正在读
            {
                LCD_ShowString(30,190,200,16,16,"USB Reading...");//提示USB正在读出数据
            }
            if(USB_STATUS_REG&0x04)
				LCD_ShowString(30,210,200,16,16,"USB Write Err ");//提示写入错误
            else 
				LCD_Fill(30,210,240,210+16,WHITE);//清除显示
            if(USB_STATUS_REG&0x08)
				LCD_ShowString(30,230,200,16,16,"USB Read  Err ");//提示读出错误
            else LCD_Fill(30,230,240,230+16,WHITE);//清除显示
            USB_STA=USB_STATUS_REG;//记录最后的状态
        }
        if(Device_STA!=USB_GetStatus())
        {
            Device_STA = USB_GetStatus();//记录新的状态
            if(Device_STA==USBD_STATE_CONFIGURED)
            {
                POINT_COLOR=BLUE;
                LCD_ShowString(30,170,200,16,16,"USB Connected    ");//提示USB连接成功
            } else
            {
                POINT_COLOR=RED;
                LCD_ShowString(30,170,200,16,16,"USB disConnected ");//提示USB断开
            }
        }
        tct++;
        if(tct==200)
        {
            tct=0;
            LED0=!LED0;//提示系统在运行
            if(USB_STATUS_REG&0x10)
            {
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=USBD_STATE_CONFIGURED;
            } else//没有得到轮询
            {
                offline_cnt++;
                if(offline_cnt>10)
				{
					bDeviceState=USBD_STATE_SUSPENDED;//2s内没收到在线标记,代表USB被拔出了
				}
                	
            }
            USB_STATUS_REG=0;
        }
    };
}


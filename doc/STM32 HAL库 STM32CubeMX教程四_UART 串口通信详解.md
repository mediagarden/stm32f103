# [UART 串口通信详解](https://www.guyuehome.com/36005)

# 前言： 

今天我们学习STM32CubeMX串口的操作，以及HAL库串口的配置，我们会详细的讲解各个模块的使用和具体功能，并且基于HAL库实现Printf函数功能重定向，UART中断接收，**本系列教程将HAL库与STM32CubeMX结合在一起讲解**，使您可以更快速的学会各个模块的使用

**所用工具**：

1、芯片： STM32F407ZET6

2、STM32CubeMx软件

3、IDE： MDK-Keil软件

4、STM32F1xx/STM32F4xxHAL库 

5、**串口： 使用USART1 PA9,PA10**

**知识概括：**

**通过本篇博客您将学到：**

STM32CubeMX创建串口例程

HAL库UATR函数库

重定义printf函数

HAL库，UART中断接收

HAL库UATR接收与发送例程

### **工程创建**

**1设置RCC**

- **设置高速外部时钟HSE 选择外部时钟源**

![img](https://img-blog.csdnimg.cn/20190810145615696.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70)

**2设置串口**

![img](https://img-blog.csdnimg.cn/20190810150549328.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70)

- 1点击USATR1  
- 2设置MODE为**异步通信(Asynchronous)**    
- **3**基础参数**：波特率为115200 Bits/s。传输数据长度为8 Bit。奇偶检验无，停止位1   接收和发送都使能** 
- 4GPIO引脚设置 USART1_RX/USART_TX
- 5 NVIC Settings 一栏使能接收中断

![img](https://img-blog.csdnimg.cn/20191011164320407.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70)

**3设置时钟**

![img](https://img-blog.csdnimg.cn/20190810151708412.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70)

**我的是 外部晶振为8MHz** 

- 1选择外部时钟HSE 8MHz  
- 2PLL锁相环倍频72倍
- 3系统时钟来源选择为PLL
- 4设置APB1分频器为 /2

**32的时钟树框图** 如果不懂的话请看[《【STM32】系统时钟RCC详解(超详细，超全面)》](https://blog.csdn.net/as480133937/article/details/98845509)

 

**4项目文件设置**

![img](https://img-blog.csdnimg.cn/20190809210713570.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70)

- 1 设置项目名称
- 2 设置存储路径
- 3 选择所用IDE

![img](https://img-blog.csdnimg.cn/2019080921100765.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70)

**5创建工程文件**

然后点击**GENERATE CODE** 创建工程

### 配置下载工具

新建的工程所有配置都是默认的 我们需要自行选择下载模式，勾选上下载后复位运行

### ![img](https://img-blog.csdnimg.cn/20190809172359875.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70)

# HAL库UART函数库介绍

 **UART结构体定义**

```cpp
UART_HandleTypeDef huart1;
```

**UART的名称定义**，这个结构体中存放了**UART所有用到的功能**，后面的别名就是我们所用的uart串口的别名，默认为**huart1**

可以自行修改

![img](https://img-blog.csdnimg.cn/20190811085620805.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70)

1、串口发送/接收函数

- **HAL_UART_Transmit**();串口发送数据，使用超时管理机制 
- **HAL_UART_Receive**();串口接收数据，使用超时管理机制
- **HAL_UART_Transmit_IT**();串口中断模式发送 
- **HAL_UART_Receive_IT**();串口中断模式接收
- **HAL_UART_Transmit_DMA**();串口DMA模式发送
- **HAL_UART_Transmit_DMA**();串口DMA模式接收

这几个函数的参数基本都是一样的，我们挑两个讲解一下

**串口发送数据：**

```cpp
HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
```

功能：**串口**发送指定长度的数据。如果超时没发送完成，则不再发送，返回超时标志（HAL_TIMEOUT）。

参数：

- UART_HandleTypeDef *huart   **UATR的别名**  如 :  UART_HandleTypeDef huart1;  别名就是huart1 
- *pData   **需要发送的数据** 
- Size  **发送的字节数**
- Timeout  **最大发送时间**，发送数据超过该时间退出发送  

```cpp
HAL_UART_Transmit(&huart1, (uint8_t *)ZZX, 3, 0xffff);  //串口发送三个字节数据，最大传输时间0xffff
```

**中断接收数据：**

```cpp
HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
```

功能：**串口中断接收，以中断方式接收指定长度数据。
大致过程是，设置数据存放位置，接收数据长度，然后使能串口接收中断。接收到数据时，会触发串口中断。
再然后，串口中断函数处理，直到接收到指定长度数据，而后关闭中断，进入中断接收回调函数，不再触发接收中断。(只触发一次中断)**

参数：

- UART_HandleTypeDef *huart   **UATR的别名**  如 :  UART_HandleTypeDef huart1;  别名就是huart1 
- *pData   **接收到的数据存放地址**
- Size  **接收的字节数**

```cpp
HAL_UART_Receive_IT(&huart1,(uint8_t *)&value,1);   //中断接收一个字符，存储到value中
```

2、串口中断函数

- **HAL_UART_IRQHandler**(UART_HandleTypeDef *huart); //串口中断处理函数
- **HAL_UART_TxCpltCallback**(UART_HandleTypeDef *huart); //串口发送中断回调函数
- **HAL_UART_TxHalfCpltCallback**(UART_HandleTypeDef *huart); //串口发送一半中断回调函数（用的较少）
- **HAL_UART_RxCpltCallback**(UART_HandleTypeDef *huart); //串口接收中断回调函数
- **HAL_UART_RxHalfCpltCallback**(UART_HandleTypeDef *huart);//串口接收一半回调函数（用的较少）
- **HAL_UART_ErrorCallback**();串口接收错误函数

**串口接收中断回调函数：**

```cpp
HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);  
```

功能：HAL库的中断进行完之后，并不会直接退出，而是会进入中断回调函数中，用户可以在其中设置代码，

​      **串口中断接收完成之后，会进入该函数**，该函数为空函数，用户需自行修改，

参数：

- UART_HandleTypeDef *huart   **UATR的别名**  如 :  UART_HandleTypeDef huart1;  别名就是huart1 

```cpp
HAL_UART_RxCpltCallback(&huart1){           //用户设定的代码               }
```

**串口中断处理函数**

```cpp
HAL_UART_IRQHandler(UART_HandleTypeDef *huart);  
```

功能：对接收到的数据进行判断和处理 **判断是发送中断还是接收中断**，然后进行数据的发送和接收，在中断服务函数中使用

 

如果接收数据，则会进行接收中断处理函数

```markup
 /* UART in mode Receiver ---------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    UART_Receive_IT(huart);
  }
```

如果发送数据，则会进行发送中断处理函数

```markup
  /* UART in mode Transmitter ------------------------------------------------*/
  if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    UART_Transmit_IT(huart);
    return;
  }
```

3串口查询函数

 **HAL_UART_GetState**(); 判断UART的接收是否结束，或者发送数据是否忙碌

 举例：   

```cpp
while(HAL_UART_GetState(&huart4) == HAL_UART_STATE_BUSY_TX)   //检测UART发送结束
```

 

# USART接收与发送

## 重新定义printf函数

- 在 stm32f4xx_hal.c

  中包含#include <stdio.h>

  ```markup
  #include "stm32f4xx_hal.h"
  #include <stdio.h>
  extern UART_HandleTypeDef huart1;   //声明串口
  ```

- **在 stm32f4xx_hal.c 中重写fget和fput函数**

  ```cpp
  /**
    * 函数功能: 重定向c库函数printf到DEBUG_USARTx
    * 输入参数: 无
    * 返 回 值: 无
    * 说    明：无
    */
  int fputc(int ch, FILE *f)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
    return ch;
  }
   
  /**
    * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
    * 输入参数: 无
    * 返 回 值: 无
    * 说    明：无
    */
  int fgetc(FILE *f)
  {
    uint8_t ch = 0;
    HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
    return ch;
  }
  ```

  **在main.c中添加**

  ```cpp
   #define RXBUFFERSIZE  256
  char RxBuffer[RXBUFFERSIZE]; 
  
    while (1)
    {
      /* USER CODE END WHILE */
  			printf("Z小旋测试\n");
  			HAL_Delay(1000);
      /* USER CODE BEGIN 3 */
    }
  ```

之后便可以使用Printf函数和Scanf，getchar函数

![img](https://img-blog.csdnimg.cn/20190810161732231.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70)



### UART接收中断

**因为中断接收函数只能触发一次接收中断，所以我们需要在中断回调函数中再调用一次中断接收函数**

### 具体流程：

1、初始化串口

2、在main中第一次调用接收中断函数

3、进入接收中断，接收完数据 进入中断回调函数

4、修改HAL_UART_RxCpltCallback中断回调函数，处理接收的数据，

5 **回调函数中要调用一次HAL_UART_Receive_IT函数，使得程序可以重新触发接收中断**

**函数流程图：**

HAL_UART_Receive_IT(**中断接收函数**)   **->** USART2_IRQHandler(void)(**中断服务函数**)  ->  HAL_UART_IRQHandler(UART_HandleTypeDef *huart)(**中断处理函数**)  ->  UART_Receive_IT(UART_HandleTypeDef *huart) (**接收函数**)  ->  HAL_UART_RxCpltCallback(huart);(**中断回调函数**)

HAL_UART_RxCpltCallback函数就是用户要重写在main.c里的回调函数。

**代码实现：**

  并在main.c中添加下列定义：

```cpp
#include <string.h>
 
#define RXBUFFERSIZE  256     	//最大接收字节数
char RxBuffer[RXBUFFERSIZE];   	//接收数据
uint8_t aRxBuffer;				//接收中断缓冲
uint8_t Uart1_Rx_Cnt = 0;		//接收缓冲计数
```

在main()主函数中，调用一次接收中断函数

```cpp
/* USER CODE BEGIN 2 */
HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
/* USER CODE END 2 */
```

在main.c下方添加中断回调函数

```cpp
/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
 
	if(Uart1_Rx_Cnt >= 255)  //溢出判断
	{
		Uart1_Rx_Cnt = 0;
		memset(RxBuffer,0x00,sizeof(RxBuffer));
		HAL_UART_Transmit(&huart1, (uint8_t *)"数据溢出", 10,0xFFFF); 	
        
	}
	else
	{
		RxBuffer[Uart1_Rx_Cnt++] = aRxBuffer;   //接收数据转存
	
		if((RxBuffer[Uart1_Rx_Cnt-1] == 0x0A)&&(RxBuffer[Uart1_Rx_Cnt-2] == 0x0D)) //判断结束位
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, Uart1_Rx_Cnt,0xFFFF); //将收到的信息发送出去
            while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
			Uart1_Rx_Cnt = 0;
			memset(RxBuffer,0x00,sizeof(RxBuffer)); //清空数组
		}
	}
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断
}
/* USER CODE END 4 */

```

发送数据被正常返回

![img](https://img-blog.csdnimg.cn/20190810203850858.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70)
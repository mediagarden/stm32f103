/* USER CODE BEGIN Header */
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stddef.h>
#include <stdarg.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//@ref:https://github.com/mpredfearn/simple-printf
int putchar(int c);
static void simple_outputchar(char **str, char c) {
	if (str) {
		**str = c;
		++(*str);
	} else {
		putchar(c);
	}
}

enum flags {
	PAD_ZERO = 1, PAD_RIGHT = 2,
};

static int prints(char **out, const char *string, int width, int flags) {
	int pc = 0, padchar = ' ';

	if (width > 0) {
		int len = 0;
		const char *ptr;
		for (ptr = string; *ptr; ++ptr)
			++len;
		if (len >= width)
			width = 0;
		else
			width -= len;
		if (flags & PAD_ZERO)
			padchar = '0';
	}
	if (!(flags & PAD_RIGHT)) {
		for (; width > 0; --width) {
			simple_outputchar(out, padchar);
			++pc;
		}
	}
	for (; *string; ++string) {
		simple_outputchar(out, *string);
		++pc;
	}
	for (; width > 0; --width) {
		simple_outputchar(out, padchar);
		++pc;
	}

	return pc;
}

#define PRINT_BUF_LEN 64

static int simple_outputi(char **out, long long i, int base, int sign,
		int width, int flags, int letbase) {
	char print_buf[PRINT_BUF_LEN];
	char *s;
	int t, neg = 0, pc = 0;
	unsigned long long u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints(out, print_buf, width, flags);
	}

	if (sign && base == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	while (u) {
		t = u % base;
		if (t >= 10)
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= base;
	}

	if (neg) {
		if (width && (flags & PAD_ZERO)) {
			simple_outputchar(out, '-');
			++pc;
			--width;
		} else {
			*--s = '-';
		}
	}

	return pc + prints(out, s, width, flags);
}

static int simple_vsprintf(char **out, char *format, va_list ap) {
	int width, flags;
	int pc = 0;
	char scr[2];
	union {
		char c;
		char *s;
		int i;
		unsigned int u;
		long li;
		unsigned long lu;
		long long lli;
		unsigned long long llu;
		short hi;
		unsigned short hu;
		signed char hhi;
		unsigned char hhu;
		void *p;
	} u;

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = flags = 0;
			if (*format == '\0')
				break;
			if (*format == '%')
				goto out;
			if (*format == '-') {
				++format;
				flags = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				flags |= PAD_ZERO;
			}
			if (*format == '*') {
				width = va_arg(ap, int);
				format++;
			} else {
				for (; *format >= '0' && *format <= '9'; ++format) {
					width *= 10;
					width += *format - '0';
				}
			}
			switch (*format) {
			case ('d'):
				u.i = va_arg(ap, int);
				pc += simple_outputi(out, u.i, 10, 1, width, flags, 'a');
				break;

			case ('u'):
				u.u = va_arg(ap, unsigned int);
				pc += simple_outputi(out, u.u, 10, 0, width, flags, 'a');
				break;

			case ('x'):
				u.u = va_arg(ap, unsigned int);
				pc += simple_outputi(out, u.u, 16, 0, width, flags, 'a');
				break;

			case ('X'):
				u.u = va_arg(ap, unsigned int);
				pc += simple_outputi(out, u.u, 16, 0, width, flags, 'A');
				break;

			case ('c'):
				u.c = va_arg(ap, int);
				scr[0] = u.c;
				scr[1] = '\0';
				pc += prints(out, scr, width, flags);
				break;

			case ('s'):
				u.s = va_arg(ap, char*);
				pc += prints(out, u.s ? u.s : "(null)", width, flags);
				break;
			case ('l'):
				++format;
				switch (*format) {
				case ('d'):
					u.li = va_arg(ap, long);
					pc += simple_outputi(out, u.li, 10, 1, width, flags, 'a');
					break;

				case ('u'):
					u.lu = va_arg(ap, unsigned long);
					pc += simple_outputi(out, u.lu, 10, 0, width, flags, 'a');
					break;

				case ('x'):
					u.lu = va_arg(ap, unsigned long);
					pc += simple_outputi(out, u.lu, 16, 0, width, flags, 'a');
					break;

				case ('X'):
					u.lu = va_arg(ap, unsigned long);
					pc += simple_outputi(out, u.lu, 16, 0, width, flags, 'A');
					break;

				case ('l'):
					++format;
					switch (*format) {
					case ('d'):
						u.lli = va_arg(ap, long long);
						pc += simple_outputi(out, u.lli, 10, 1, width, flags,
								'a');
						break;

					case ('u'):
						u.llu = va_arg(ap, unsigned long long);
						pc += simple_outputi(out, u.llu, 10, 0, width, flags,
								'a');
						break;

					case ('x'):
						u.llu = va_arg(ap, unsigned long long);
						pc += simple_outputi(out, u.llu, 16, 0, width, flags,
								'a');
						break;

					case ('X'):
						u.llu = va_arg(ap, unsigned long long);
						pc += simple_outputi(out, u.llu, 16, 0, width, flags,
								'A');
						break;

					default:
						break;
					}
					break;
				default:
					break;
				}
				break;
			case ('h'):
				++format;
				switch (*format) {
				case ('d'):
					u.hi = va_arg(ap, int);
					pc += simple_outputi(out, u.hi, 10, 1, width, flags, 'a');
					break;

				case ('u'):
					u.hu = va_arg(ap, unsigned int);
					pc += simple_outputi(out, u.lli, 10, 0, width, flags, 'a');
					break;

				case ('x'):
					u.hu = va_arg(ap, unsigned int);
					pc += simple_outputi(out, u.lli, 16, 0, width, flags, 'a');
					break;

				case ('X'):
					u.hu = va_arg(ap, unsigned int);
					pc += simple_outputi(out, u.lli, 16, 0, width, flags, 'A');
					break;

				case ('h'):
					++format;
					switch (*format) {
					case ('d'):
						u.hhi = va_arg(ap, int);
						pc += simple_outputi(out, u.hhi, 10, 1, width, flags,
								'a');
						break;

					case ('u'):
						u.hhu = va_arg(ap, unsigned int);
						pc += simple_outputi(out, u.lli, 10, 0, width, flags,
								'a');
						break;

					case ('x'):
						u.hhu = va_arg(ap, unsigned int);
						pc += simple_outputi(out, u.lli, 16, 0, width, flags,
								'a');
						break;

					case ('X'):
						u.hhu = va_arg(ap, unsigned int);
						pc += simple_outputi(out, u.lli, 16, 0, width, flags,
								'A');
						break;

					default:
						break;
					}
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		} else {
			out: simple_outputchar(out, *format);
			++pc;
		}
	}
	if (out)
		**out = '\0';
	return pc;
}

int simple_printf(char *fmt, ...) {
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = simple_vsprintf(NULL, fmt, ap);
	va_end(ap);

	return r;
}

int simple_sprintf(char *buf, char *fmt, ...) {
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = simple_vsprintf(&buf, fmt, ap);
	va_end(ap);

	return r;
}

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define KEY0 HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4) 	//KEY0 按键 PE4
#define KEY1 HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3) 	//KEY1 按键 PE3
#define KEY2 HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2) 	//KEY2 按键 PE2
#define WK_UP HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) 	//WKUP 按键 PA0

#define KEY0_PRES (1)
#define KEY1_PRES (1<<1)
#define KEY2_PRES (1<<2)
#define WKUP_PRES (1<<3)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
enum KEY_PRESS_MODE {
	KEY_PRESS_SHORT_MODE, KEY_PRESS_LONG_MODE,
};
uint8_t Key_Scan(enum KEY_PRESS_MODE mode) {
	static uint8_t key_up = 1;
	if (mode == KEY_PRESS_SHORT_MODE) {
		key_up = 1;
	}

	if (key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1)) {
		HAL_Delay(10);
		key_up = 0;
		if (KEY0 == 0) {
			return KEY0_PRES;
		} else if (KEY1 == 0) {
			return KEY1_PRES;
		} else if (KEY2 == 0) {
			return KEY2_PRES;
		} else if (WK_UP == 1) {
			return WKUP_PRES;
		}
	} else if (KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0) {
		key_up = 1;
	}
	return 0;

}

int putchar(int c) {
	HAL_UART_Transmit(&huart1, &c, 1, 10);
	return c;
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : KEY2_Pin KEY1_Pin KEY0_Pin */
	GPIO_InitStruct.Pin = KEY2_Pin | KEY1_Pin | KEY0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : LED1_Pin */
	GPIO_InitStruct.Pin = LED1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : WKUP_Pin */
	GPIO_InitStruct.Pin = WKUP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(WKUP_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LED0_Pin */
	GPIO_InitStruct.Pin = LED0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI2_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	HAL_NVIC_SetPriority(EXTI3_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == WKUP_Pin) {
		if (Key_Scan(KEY_PRESS_LONG_MODE) == WKUP_PRES) {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);
		}
	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

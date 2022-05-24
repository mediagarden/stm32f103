#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "sys.h"

void uart_init(uint32_t baudRate);

int simple_printf(char *fmt, ...);
int simple_sprintf(char *buf, char *fmt, ...);


#endif



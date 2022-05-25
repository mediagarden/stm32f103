#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "sys.h"

#ifdef UART_RX_ENABLE
#define RX_BUFFER_SIZE  256

extern uint8_t huart1_rx_buffer[RX_BUFFER_SIZE];
extern uint16_t huart1_rx_len;
#endif

void uart_init(uint32_t baudRate);

#ifdef UART_RX_ENABLE
void uart_begin_async_recv();
void uart_end_async_recv();
int uart_finish_packet();
#endif


int simple_printf(char *fmt, ...);
int simple_sprintf(char *buf, char *fmt, ...);


#endif



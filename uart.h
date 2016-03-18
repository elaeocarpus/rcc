/*
 * uart.h
 *
 *
 */

#ifndef _UART_H
#define _UART_H

#include "stm32f103xb.h"

void uart2_init(void);
void uart2_send_byte(uint8_t tx_data);
void usart2_rcv_byte(void);
int usart2_rxdata_rdy(void);
uint8_t usart2_read_byte(void);
uint8_t usart2_read(void);

#endif

#ifndef __USART1_H
#define __USART1_H

#include "stm32f10x.h"

void usart1_init(u32 baud);
void usart1_send_str(u8 *data);
void usart1_send_str_len(u8 *data, u8 len);
void usart1_rx_monitor(u16 time);
void usart1_driver(void);
void usart1_send_str_u16(u16 data);

#endif

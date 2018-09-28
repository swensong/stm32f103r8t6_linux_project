#ifndef _SYSTICK_H
#define _SYSTICK_H

#include "stm32f10x.h"

#define SYSTICK_PICK 1000

void systick_init(void);
void delay_ms(int ms);
void delay_us(int us);

#endif

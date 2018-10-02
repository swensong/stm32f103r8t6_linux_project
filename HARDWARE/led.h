#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include "sys.h"

/* 定义PA5为LD2灯的接口 */
#define LED1 PAout(5)
#define LED PCout(13)

/**	当表示为LIGHT时候LED灯点亮，当表示为DARK时候LED灯点亮 */
#define LIGHT 1
#define DARK	0

void led_init(void);
void led_flash_light(u16 time, u8 led_enable);
void led_hc_sr04(u8 time);

#endif

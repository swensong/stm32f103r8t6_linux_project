#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

#include "stm32f10x.h"
#include "led.h"

/*
  PB5 作为触发口
  PB4 作为接收触发口
 */
#define TRIG	PBout(5)

void ultrasonic_init(void);
float get_ultrasonic_distanse(void);
void ultrasonic_monitor(u16 time);

#endif

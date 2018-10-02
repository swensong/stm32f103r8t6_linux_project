#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

#include "stm32f10x.h"

void ultrasonic_init(void);
float get_ultrasonic_distanse(void);

#endif

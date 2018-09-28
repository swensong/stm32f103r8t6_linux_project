#ifndef _ADC_DMA_H
#define _ADC_DMA_H

#include "stm32f10x.h"

/* 固定一个通道的版本 */

void adc1_init(void);
u16 get_adc1_value(void);

void adc_test(void);

#endif

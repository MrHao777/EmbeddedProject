#ifndef _adc_H
#define _adc_H

#include "stm32f1xx_hal.h"

//根据实际情况调节
#define ADC_VALUE_MAX 4095
#define ADC_VALUE_MIN 1032

void JX_ADC_Init(void);
void JX_ADC_DeInit(void);
uint32_t JX_ADC_GetPower(void);

#endif

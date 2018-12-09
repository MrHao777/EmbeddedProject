#ifndef _shocksensor_H
#define _shocksensor_H

#include "stm32f1xx_hal.h"

extern uint8_t shock_sensor_state;

void JX_Shock_Init(void);
void JX_Shock_DeInit(void);
void JX_Shock_StartSensing(void);
void JX_Shock_StopSensing(void);

void EXTI4_IRQHandler(void);

#endif

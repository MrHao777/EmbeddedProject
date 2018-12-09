#ifndef _timer_H
#define _timer_H

#include "stm32f1xx_hal.h"

//由定时器中断置1, 状态机清零
extern uint8_t timer_alarm_state;

void JX_TimerInit(void);
void JX_TimerDeInt(void);
void JX_TimerSetAlarmTime(uint8_t time);
void JX_TimerStop(void);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void TIM1_UP_IRQHandler(void);

#endif

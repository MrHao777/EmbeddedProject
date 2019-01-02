#ifndef _power_H
#define _power_H

#include "stm32f1xx_hal.h"

void JX_PowerCtrlInit(void);
void JX_PowerCtrlDeInit(void);
void JX_PowerON(void);
void JX_PowerOFF(void);
void JX_EnterStandbyMode(void);
void JX_Time4Init(void);
void JX_Timer4Deinit(void);
void JX_Timer4SetAlarm(void);
void JX_Timer4ClearCount(void);

#endif

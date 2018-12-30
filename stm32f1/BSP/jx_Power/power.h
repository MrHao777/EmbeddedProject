#ifndef _power_H
#define _power_H

#include "stm32f1xx_hal.h"

void JX_PowerCtrlInit(void);
void JX_PowerCtrlDeInit(void);
void JX_PowerON(void);
void JX_PowerOFF(void);

#endif

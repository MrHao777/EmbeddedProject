#ifndef _microswitch_H
#define _microswitch_H

#include "stm32f1xx_hal.h"

void JX_DoorState_Init(void);
void JX_DoorState_DeInit(void);
int32_t JX_GetDoorState(void);

#endif

#ifndef _microswitch_H
#define _microswitch_H

#include "stm32f1xx_hal.h"

extern uint8_t door_open_timeout_state;
	
void JX_DoorState_Init(void);
void JX_DoorState_DeInit(void);
int32_t JX_GetDoorState(void);

void JX_Timer3Init(void);
void JX_Timer3Deinit(void);

void JX_Timer3SetAlarm(void);
void JX_Timer3Stop(void);

#endif

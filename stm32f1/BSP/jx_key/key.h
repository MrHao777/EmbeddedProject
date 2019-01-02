#ifndef _key_H
#define _key_H

#include "stm32f1xx_hal.h"

extern uint8_t key_wakeup_state;
extern uint8_t key_reset_state;
extern uint8_t key_set_state;
extern uint8_t key_Pressed;


void JX_KeyInit(void);
void JX_KeyDeInit(void);


void JX_Time2Init(void);
void JX_Time2Deinit(void);

void TIM2_IRQHandler(void);

//void EXTI1_IRQHandler(void);
//void EXTI2_IRQHandler(void);
//void EXTI3_IRQHandler(void);

#endif

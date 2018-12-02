#ifndef _LED_H
#define _LED_H

#include "stm32h7xx_hal.h"


#define LED_Pin GPIO_PIN_10
#define LED_GPIO_Port GPIOF

#define JX_LED_OFF   		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET)
#define JX_LED_ON    		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET)
#define JX_LED_Toggle    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin)
void JX_LED_Init(void);

#endif

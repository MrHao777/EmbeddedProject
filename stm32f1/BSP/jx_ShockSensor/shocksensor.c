#include "ShockSensor.h"

uint8_t shock_sensor_state = 0;

void JX_Shock_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();

	//¸´Î»¼ü
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI4_IRQn, 3, 3);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

void JX_Shock_DeInit(void)
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
	HAL_NVIC_DisableIRQ(EXTI4_IRQn);
}

void JX_Shock_StartSensing(void)
{
	
}

void JX_Shock_StopSensing(void)
{
	
}

void EXTI4_IRQHandler(void)
{
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
//	shock_sensor_state = 1;
}


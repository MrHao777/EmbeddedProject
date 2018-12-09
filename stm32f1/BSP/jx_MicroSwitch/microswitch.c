#include "microswitch.h"

void JX_DoorState_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void JX_DoorState_DeInit(void)
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
}

int32_t JX_GetDoorState(void)
{
	return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3);
}

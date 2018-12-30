#include "microswitch.h"
#include "delay.h"

static uint8_t door_state = 0;

void JX_DoorState_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI4_IRQn, 3, 3);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

void JX_DoorState_DeInit(void)
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
	HAL_NVIC_DisableIRQ(EXTI4_IRQn);
}

int32_t JX_GetDoorState(void)
{
	return door_state;
}

void EXTI4_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
	delay_ms(50);
	door_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
	if(door_state)
	  printf("door open \n\r");
	else
		printf("door close \n\r");
}

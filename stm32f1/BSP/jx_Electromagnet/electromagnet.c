#include "electromagnet.h"
#include "microswitch.h"
#include "delay.h"

void JX_OpenDoor_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
}

void JX_OpenDoor_DeInit(void)
{
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5);
}

void JX_ExecuteOpenDoor(void)
{
	uint8_t time_out=0;
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	while(JX_GetDoorState() == 0 && time_out<200)
	{
		delay_ms(100);
		time_out ++;
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
}



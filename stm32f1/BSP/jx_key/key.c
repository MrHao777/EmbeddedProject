#include "key.h"
#include "delay.h"

uint8_t key_reset_state = 0;
uint8_t key_set_state = 0;
uint8_t key_wakeup_state = 0;

void JX_KeyInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI0_IRQn, 3, 3);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 3);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI2_IRQn, 3, 3);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

}

void JX_KeyDeInit(void)
{
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
}

void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  key_wakeup_state = 1;
	delay_ms(50);
	printf("week up \n\r");
}

void EXTI1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
	key_set_state = 1;
	delay_ms(50);	
	printf("set \n\r");
}

void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
	key_reset_state = 1;
	delay_ms(50);	
	printf("reset \n\r");
}





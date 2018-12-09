#include "key.h"

uint8_t key_reset_state = 0;
uint8_t key_set_state = 0;
uint8_t key_wakeup_state = 0;

void JX_KeyInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();

	//¸´Î»¼ü
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 3);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	//ÉèÖÃ¼ü
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI2_IRQn, 3, 3);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	//»½ÐÑ¼ü
//  GPIO_InitStruct.Pin = GPIO_PIN_3;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//  GPIO_InitStruct.Pull = GPIO_PULLUP;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//  HAL_NVIC_SetPriority(EXTI3_IRQn, 3, 3);
//  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

void JX_KeyDeInit(void)
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
//	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);
//	HAL_NVIC_DisableIRQ(EXTI3_IRQn);
}

void EXTI1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
	key_reset_state = 1;
}

void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
	key_set_state = 1;
}

//void EXTI3_IRQHandler(void)
//{
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
//  key_wakeup_state = 1;
//}




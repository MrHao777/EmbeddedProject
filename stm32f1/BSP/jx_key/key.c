#include "key.h"
#include "delay.h"
#include "usart.h"

uint8_t key_reset_state = 0;
uint8_t key_set_state = 0;
uint8_t key_wakeup_state = 0;
uint8_t key_Pressed = 0;

static TIM_HandleTypeDef htim2;

void JX_KeyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//10msÂÖÑ¯	
	JX_Time2Init();
}

void JX_KeyDeInit(void)
{
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
	JX_Time2Deinit();
}

void JX_Time2Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  //64M
	__HAL_RCC_TIM2_CLK_ENABLE();
	
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 79;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }	
	
	HAL_NVIC_SetPriority(TIM2_IRQn, 3, 3);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	
  if (	HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  if (	HAL_TIM_Base_Start(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void JX_Time2Deinit(void)
{
	__HAL_RCC_TIM2_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
	static uint8_t key_WkUp_last_state = 0, key_Set_last_state = 0;
	static uint16_t key_Reset_last_state = 0;
	uint8_t key_value;
	
  HAL_TIM_IRQHandler(&htim2);
	
	key_value = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	if(key_WkUp_last_state == 0 && key_value == 1)
	{
		key_wakeup_state = 1;
		key_Pressed = 1;
	}
	key_WkUp_last_state = key_value;
	
	key_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
	if(key_Set_last_state == 0 && key_value == 1)
	{
		key_set_state = 1;
		key_Pressed = 1;
	}
	key_Set_last_state = key_value;

	key_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
	if(key_value)
	{
		key_Reset_last_state += 1;
		key_Pressed = 1;
	}
	else
		key_Reset_last_state = 0;
	if(key_Reset_last_state >= 500)
	{
		key_Reset_last_state = 0;
		key_reset_state = 1;
	}
}

//void JX_KeyInit(void)
//{
//  GPIO_InitTypeDef GPIO_InitStruct;

//  __HAL_RCC_GPIOA_CLK_ENABLE();
//  __HAL_RCC_GPIOB_CLK_ENABLE();

//	GPIO_InitStruct.Pin = GPIO_PIN_0;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//  HAL_NVIC_SetPriority(EXTI0_IRQn, 3, 3);
//  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

//  GPIO_InitStruct.Pin = GPIO_PIN_1;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//  HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 3);
//  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
//	
//  GPIO_InitStruct.Pin = GPIO_PIN_2;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//  HAL_NVIC_SetPriority(EXTI2_IRQn, 3, 3);
//  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

//}

//void JX_KeyDeInit(void)
//{
//	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
//	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
//	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
//	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
//	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
//	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
//}

//void EXTI0_IRQHandler(void)
//{
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
//  key_wakeup_state = 1;
//	delay_ms(50);
//	printf("week up \n\r");
//}

//void EXTI1_IRQHandler(void)
//{
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
//	key_set_state = 1;
//	delay_ms(50);	
//	printf("set \n\r");
//}

//void EXTI2_IRQHandler(void)
//{
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
//	key_reset_state = 1;
//	delay_ms(50);	
//	printf("reset \n\r");
//}





#include "microswitch.h"
#include "delay.h"

static uint8_t door_state = 0;
uint8_t door_open_timeout_state = 0;

static TIM_HandleTypeDef htim3;

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
	
	JX_Timer3Init();
}

void JX_DoorState_DeInit(void)
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
	HAL_NVIC_DisableIRQ(EXTI4_IRQn);
	JX_Timer3Deinit();
}

int32_t JX_GetDoorState(void)
{
	return door_state;
}

void JX_Timer3Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  //64M
	__HAL_RCC_TIM3_CLK_ENABLE();
	
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 65530;  //65535
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65530;     //65535
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }	
	
	HAL_NVIC_SetPriority(TIM3_IRQn, 3, 3);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void JX_Timer3Deinit(void)
{
	__HAL_RCC_TIM3_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
}

void JX_Timer3SetAlarm(void)
{
	TIM3->EGR = 1;
	__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
  if (	HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  if (	HAL_TIM_Base_Start(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void JX_Timer3Stop(void)
{
  if (	HAL_TIM_Base_Stop_IT(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	TIM3->EGR = 1;
  if (	HAL_TIM_Base_Stop(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim3);
	HAL_TIM_Base_Stop_IT(&htim3);
	HAL_TIM_Base_Stop(&htim3);
	door_open_timeout_state = 1;
}

void EXTI4_IRQHandler(void)
{	
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
	delay_ms(50);
	door_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
	if(door_state)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
		JX_Timer3Stop();
		JX_Timer3SetAlarm();
	}
	else
	{
		JX_Timer3Stop();
	}
}

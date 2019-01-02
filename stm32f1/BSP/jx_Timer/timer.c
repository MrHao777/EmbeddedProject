#include "timer.h"

static TIM_HandleTypeDef htim1;

uint8_t timer_alarm_state = 0;

void JX_TimerInit(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

	__HAL_RCC_TIM1_CLK_ENABLE();
  
	//64M
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7999;  //16λ
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 7999;     //16λ
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; //0 2 4
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
	HAL_NVIC_SetPriority(TIM1_UP_IRQn, 3, 2);
  HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
}

void JX_TimerDeInt(void)
{
    __HAL_RCC_TIM1_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
}

//0-255 0代表1s
void JX_TimerSetAlarmTime(uint8_t time)
{
	htim1.Init.RepetitionCounter = time;
  if (	HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	TIM1->EGR = 1;
	__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);
  if (	HAL_TIM_Base_Start_IT(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  if (	HAL_TIM_Base_Start(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void JX_TimerStop(void)
{
  if (	HAL_TIM_Base_Stop_IT(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  if (	HAL_TIM_Base_Stop(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void TIM1_UP_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim1);
	HAL_TIM_Base_Stop_IT(&htim1);
	HAL_TIM_Base_Stop(&htim1);
	timer_alarm_state = 1;
}

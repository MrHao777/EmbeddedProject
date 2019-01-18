#include "power.h"
#include "usart.h"

static TIM_HandleTypeDef htim4;

void JX_PowerCtrlInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOA_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	JX_Time4Init();
//	JX_Timer4SetAlarm();
}

void JX_PowerCtrlDeInit(void)
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11);	
}

void JX_PowerON(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);	
}

void JX_PowerOFF(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);	
}

uint8_t JX_GetPowerState(void)
{
	return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);
}

void JX_EnterStandbyMode(void)
{
	__HAL_RCC_APB2_FORCE_RESET();      
	__HAL_RCC_PWR_CLK_ENABLE();         
				
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);               
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);        
	HAL_PWR_EnterSTANDBYMode();
}

void JX_Time4Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  //64M
	__HAL_RCC_TIM4_CLK_ENABLE();
	
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 8000*5 - 1; 
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 8000*3 - 1;     
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }	
	
	HAL_NVIC_SetPriority(TIM4_IRQn, 3, 3);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

void JX_Timer4Deinit(void)
{
	__HAL_RCC_TIM4_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(TIM4_IRQn);
}

void JX_Timer4SetAlarm(void)
{
	TIM4->EGR = 1;
	__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
  if (	HAL_TIM_Base_Start_IT(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  if (	HAL_TIM_Base_Start(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void JX_Timer4ClearCount(void)
{
  if (	HAL_TIM_Base_Stop_IT(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	TIM4->EGR = 1;
  if (	HAL_TIM_Base_Stop(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }	
	__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
  if (	HAL_TIM_Base_Start_IT(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  if (	HAL_TIM_Base_Start(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim4);
	HAL_TIM_Base_Stop_IT(&htim4);
	HAL_TIM_Base_Stop(&htim4);
	JX_EnterStandbyMode();
}



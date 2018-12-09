#include "adc.h"

static ADC_HandleTypeDef hadc1;

void JX_ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig;
  GPIO_InitTypeDef GPIO_InitStruct;
	
  __HAL_RCC_ADC1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void JX_ADC_DeInit(void)
{
	__HAL_RCC_ADC1_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);
}

uint32_t JX_ADC_GetPower(void)
{
	float temp;
	uint32_t adc_value = 0;
	uint8_t i;
	
	for(i=0; i<5 ;i++)
	{
		if (HAL_ADC_Start(&hadc1) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		adc_value += HAL_ADC_GetValue(&hadc1);
	}
	
	temp = (1.0*adc_value/5 - 1.0*ADC_VALUE_MIN)/(1.0*ADC_VALUE_MAX - 1.0*ADC_VALUE_MIN) * 100;
	adc_value = temp;
	
	return adc_value;
}



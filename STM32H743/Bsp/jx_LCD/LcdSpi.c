#include "LcdSpi.h"

void JX_LcdSpi_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	
  GPIO_InitStruct.Pin = LcdSpi_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LcdSpi_CS_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LcdSpi_SCL_Pin;
  HAL_GPIO_Init(LcdSpi_SCL_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LcdSpi_SDA_Pin;
  HAL_GPIO_Init(LcdSpi_SDA_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LcdRst_Pin;
  HAL_GPIO_Init(LcdRst_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_SET);

	GPIO_InitStruct.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_SET);

  LcdRst(1);
	LcdSpi_CS(1);
	LcdSpi_SCL(1);
	LcdSpi_SDA(0);
}


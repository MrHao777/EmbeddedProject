#include "uh0002.h"

static I2C_HandleTypeDef hi2c2;

void JX_UH0002_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOF_CLK_ENABLE();
	/**I2C2 GPIO Configuration    
	PF0     ------> I2C2_SDA
	PF1     ------> I2C2_SCL 
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/* Peripheral clock enable */
	__HAL_RCC_I2C2_CLK_ENABLE();
  //100Khz
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x20201E2E;
  hi2c2.Init.OwnAddress1 = 160;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void JX_UH0002_DeInit(void)
{
	__HAL_RCC_I2C2_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0|GPIO_PIN_1);
}

int32_t JX_UH0002_SendMessage(uint8_t *senddata)
{
	return HAL_I2C_Master_Transmit(&hi2c2, 0x50, senddata, 2, 10000);
}

int32_t JX_UH0002_ReceiveMessage(uint8_t *receivedata)
{
	return HAL_I2C_Master_Receive(&hi2c2, 0x51, receivedata, 2, 10000);
}


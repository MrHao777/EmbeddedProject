#ifndef _LcdSpi_H
#define _LcdSpi_H

#include "stm32h7xx_hal.h"

#define LcdRst_Pin GPIO_PIN_15
#define LcdRst_GPIO_Port GPIOE

#define LcdSpi_CS_Pin GPIO_PIN_5
#define LcdSpi_CS_GPIO_Port GPIOF

#define LcdSpi_SCL_Pin GPIO_PIN_4
#define LcdSpi_SCL_GPIO_Port GPIOF

#define LcdSpi_SDA_Pin GPIO_PIN_3
#define LcdSpi_SDA_GPIO_Port GPIOF

#define LcdRst(n)  (n?HAL_GPIO_WritePin(LcdRst_GPIO_Port,LcdRst_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LcdRst_GPIO_Port,LcdRst_Pin,GPIO_PIN_RESET)) 
#define LcdSpi_CS(n)  (n?HAL_GPIO_WritePin(LcdSpi_CS_GPIO_Port,LcdSpi_CS_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LcdSpi_CS_GPIO_Port,LcdSpi_CS_Pin,GPIO_PIN_RESET)) 
#define LcdSpi_SCL(n) (n?HAL_GPIO_WritePin(LcdSpi_SCL_GPIO_Port,LcdSpi_SCL_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LcdSpi_SCL_GPIO_Port,LcdSpi_SCL_Pin,GPIO_PIN_RESET)) 
#define LcdSpi_SDA(n) (n?HAL_GPIO_WritePin(LcdSpi_SDA_GPIO_Port,LcdSpi_SDA_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LcdSpi_SDA_GPIO_Port,LcdSpi_SDA_Pin,GPIO_PIN_RESET)) 

void JX_LcdSpi_Init(void);

#endif

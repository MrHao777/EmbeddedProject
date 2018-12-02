#ifndef __STM32DX8I2C_H
#define __STM32DX8I2C_H

#include "stm32h7xx_hal.h"
#include "DX8_API.h"

//用户接口
void JX_DX8_Init(void);
void JX_DX8_DeInit(void);
int32_t JX_DX8_Authen(void);
int32_t JX_DX8_SetROI(unsigned char *ROI);
int32_t JX_DX8_ReadROI(unsigned char *ROI);

//内部接口
static void JX_DX8_RNG_Init(void);
unsigned char JX_DX8_SecurityEEPROMTest();

//libdx8_stm32h7库接口
void GetSoftRandom(uint32_t *random);
unsigned char dxif_transfer(unsigned char *buf, unsigned short len);

#endif

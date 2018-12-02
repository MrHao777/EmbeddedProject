#include "dxi2c_stm32.h"
#include "dx8_engineer.h"
#include <string.h>
#include <stdlib.h>

static I2C_HandleTypeDef hi2c4;
static RNG_HandleTypeDef hrng;

typedef uint8_t u8;
typedef uint32_t vu32;

void JX_DX8_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOF_CLK_ENABLE();
	/**I2C4 GPIO Configuration    
	PF14     ------> I2C4_SCL
	PF15     ------> I2C4_SDA 
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C4;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/* Peripheral clock enable */
	__HAL_RCC_I2C4_CLK_ENABLE();
  //50Khz
  hi2c4.Instance = I2C4;
  hi2c4.Init.Timing = 0x0060E3FF;
  hi2c4.Init.OwnAddress1 = 1;
  hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c4.Init.OwnAddress2 = 0;
  hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	JX_DX8_RNG_Init();
	DX8_Reset();
}

void JX_DX8_DeInit(void)
{
	__HAL_RCC_I2C4_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOF, GPIO_PIN_14|GPIO_PIN_15);
	__HAL_RCC_RNG_CLK_DISABLE();
}

int32_t JX_DX8_Authen(void)
{
   unsigned char rv;
   uint32_t random[4];
   unsigned char tmpBuf1[20] = { PPPPP };
   unsigned char tmpBuf2[20] = { KKKKK };   

   // Wakeup and Reset DX8
   rv = DX8_Reset();
   if (rv) return rv;

   // PIN Authentication
   GetSoftRandom(random); // Generate random for verify PIN
   rv = DX8_VerifyPin((uint8_t *)random,tmpBuf1);
   if (rv) return rv;

   // Host Authentication
   memset(tmpBuf1,0x00,20);
   GetSoftRandom(random);
   rv = DX8_HostAuth((uint8_t *)random,32,tmpBuf1);
   if (rv) return rv;
   Lib_HostAuth((uint8_t *)random,32,tmpBuf2,tmpBuf2);
   rv = memcmp(tmpBuf1,tmpBuf2,20);
   if (rv) return rv;

   // DX8 Sleep to save power
   rv = DX8_Sleep(); 
   if (rv) return rv;

   return 0;
}

int32_t JX_DX8_SetROI(unsigned char *ROI)
{
	 unsigned char rv;
   uint32_t buf[4];
	 unsigned char zkey[16] = {ZZZZZ0};
	 
	 // Wakeup and Reset DX8
   rv = DX8_Reset(); 
   if (rv) return rv;
	 
	 if (dx8_info.ZoneMode[0] & 0x08) // Not Lock 
	 {                                        
			// Verify Zone
			GetSoftRandom(buf); // Generate random for verify zone          
			rv = DX8_VerifyZone(0,(uint8_t *)buf,zkey);
			if (rv) return rv;
			// write Zone
			rv = DX8_WriteZone(0,0,2,ROI);
			if (rv) return rv;
		}   
   // DX8 Sleep to save power
   rv = DX8_Sleep();
   if (rv) return rv;
	 
	 return 0;
}

int32_t JX_DX8_ReadROI(unsigned char *ROI)
{
	 unsigned char rv;
   uint32_t buf[4];
	 unsigned char zkey[16] = {ZZZZZ0};
	 
	 // Wakeup and Reset DX8
   rv = DX8_Reset(); 
   if (rv) return rv;
	 if (dx8_info.ZoneMode[0] & 0x08) // Not Lock 
	 {                                        
			// Verify Zone
			GetSoftRandom(buf); // Generate random for verify zone 		 
			rv = DX8_VerifyZone(0,(uint8_t *)buf,zkey);
			if (rv) return rv;
			// write Zone
			rv = DX8_ReadZone(0,0,2,ROI);
			if (rv) return rv;
		}   
   // DX8 Sleep to save power
   rv = DX8_Sleep();
   if (rv) return rv;
	 return 0;
}
	
/*******************************************************************/
static void JX_DX8_RNG_Init(void)
{
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
	
	__HAL_RCC_RNG_CLK_ENABLE();
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

//  Wrtie and Read Security EEPROM with random number
unsigned char JX_DX8_SecurityEEPROMTest()
{
   unsigned char i,rv;
   uint32_t rdata[4];
   uint32_t wdata[4];
	 unsigned char zkey[4][16] = { {ZZZZZ0},{ZZZZZ1},{ZZZZZ2},{ZZZZZ3} };

   // Wakeup and Reset DX8
   rv = DX8_Reset(); 
   if (rv) return rv;

   // Security EEPROM write,read and check
   for (i=0; i<4; i++) {
     GetSoftRandom(wdata); // Generate the data for EEPROM writing
     if (dx8_info.ZoneMode[i] & 0x08) { // Not Lock
       if ((dx8_info.ZoneMode[i] & 0x03) != 0x03) { // EEPROM Encrypt Mode
          // Verify Zone
          GetSoftRandom(rdata); // Generate random for verify zone          
          rv = DX8_VerifyZone(i,(uint8_t *)rdata,zkey[i]);
          if (rv) return rv;
          // write Zone
          rv = DX8_WriteZone(i,0,32,(uint8_t *)wdata);
          if (rv) return rv;
          // read Zone
          memset((uint8_t *)rdata,0x00,32);
          rv = DX8_ReadZone(i,0,32,(uint8_t *)rdata);
          if (rv) return rv;
          // Check Zone
          rv = memcmp((uint8_t *)rdata,(uint8_t *)wdata,32);
          if (rv) return rv;
       }
       else { // EEPROM Normal Mode
				  // write zone
          rv = DX8_WriteZone(i,0,32,(uint8_t *)wdata);
          if (rv) return rv;
				  // read zone
          memset((uint8_t *)rdata,0x00,32);
          rv = DX8_ReadZone(i,0,32,(uint8_t *)rdata);				 
          if (rv) return rv;
				  // check zone
          rv = memcmp((uint8_t *)rdata,(uint8_t *)wdata,32);
          if (rv) return rv;
       }
     }
   }
   // DX8 Sleep to save power
   rv = DX8_Sleep();
   if (rv) return rv;

   return 0;  
}

//  Host CPU Random Number Generation
void GetSoftRandom(uint32_t *random)
{
		uint32_t RandomNumber1 = 0;
		uint32_t RandomNumber2 = 0;
	  int i;
	
		for(i = 0; i < 4; i++)
		{
			while(RandomNumber1==RandomNumber2)
			{
				HAL_RNG_GenerateRandomNumber(&hrng,&RandomNumber1);
				HAL_RNG_GenerateRandomNumber(&hrng,&RandomNumber2);
			}
			random[i] = RandomNumber2;
		}
}

unsigned char dxif_transfer(unsigned char *buf, unsigned short len)
{
	// Data
	if((buf[0] & 0x01) == 0)  //Ð´
	{
		if(HAL_I2C_Master_Transmit(&hi2c4, buf[0], buf+1, len-1, 100000)!= HAL_OK)
			return 1;
	}
	else  // ¶Á
	{		
		if(HAL_I2C_Master_Receive(&hi2c4, buf[0], buf+1, len-1, 100000)!= HAL_OK)
			return 1;
	}
	return 0;
}

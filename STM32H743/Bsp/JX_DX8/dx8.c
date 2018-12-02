#include "stm32h7xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include "dxi2c_stm32.h"

#include "DX8_API.h"
#include "dx8_engineer.h"

//#define DX82_ENABLE
static RNG_HandleTypeDef hrng;

static void MX_RNG_Init(void)
{

  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
	
	__HAL_RCC_RNG_CLK_ENABLE();
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

//  Host CPU Random Number Generation
void GetSoftRandom(uint32_t *random)
{
		uint32_t RandomNumber1 = 0;
		uint32_t RandomNumber2 = 0;
	  int i;
		MX_RNG_Init();
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

//  PIN and Host Authentication
unsigned char AuthenticationDX8()
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

//write to eeprom
unsigned char Write2EEPROM(unsigned char *wdata, unsigned char data_length)
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
			rv = DX8_WriteZone(0,0,data_length,wdata);
			if (rv) return rv;
		}   
   // DX8 Sleep to save power
   rv = DX8_Sleep();
   if (rv) return rv;
	 
	 return 0;
}

unsigned char ReadEEPROM(unsigned char *rdata, unsigned char data_length)
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
			rv = DX8_ReadZone(0,0,data_length,rdata);
			if (rv) return rv;
		}   
   // DX8 Sleep to save power
   rv = DX8_Sleep();
   if (rv) return rv;
	 return 0;
}

//  Wrtie and Read Security EEPROM with random number
unsigned char SecurityEEPROMTest()
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


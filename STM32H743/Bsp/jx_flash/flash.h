#ifndef FV_FLASH_H_LJS
#define FV_FLASH_H_LJS

#include "stm32h7xx_hal.h"

//#define RFS
//#define DATA_32                 ((uint32_t)0x12345678)
//#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_4   /* Start @ of user Flash area */
//#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_8   /* End @ of user Flash area */
//#define Sample_Size_Unit        ((uint32_t) 1024*2)

#define ADDR_FLASH_BANK1_SECTOR_0		((uint32_t)0x08000000)	/* Base @ of Bank1 Sector 0, 128 Kbytes */
#define ADDR_FLASH_BANK1_SECTOR_1		((uint32_t)0x08020000)	/* Base @ of Bank1 Sector 1, 128 Kbytes */
#define ADDR_FLASH_BANK1_SECTOR_2		((uint32_t)0x08040000)	/* Base @ of Bank1 Sector 2, 128 Kbytes */
#define ADDR_FLASH_BANK1_SECTOR_3		((uint32_t)0x08060000)	/* Base @ of Bank1 Sector 3, 128 Kbytes */
#define ADDR_FLASH_BANK1_SECTOR_4		((uint32_t)0x08080000)	/* Base @ of Bank1 Sector 4, 128 Kbytes */
#define ADDR_FLASH_BANK1_SECTOR_5		((uint32_t)0x080A0000)	/* Base @ of Bank1 Sector 5, 128 Kbytes */
#define ADDR_FLASH_BANK1_SECTOR_6		((uint32_t)0x080C0000)	/* Base @ of Bank1 Sector 6, 128 Kbytes */
#define ADDR_FLASH_BANK1_SECTOR_7		((uint32_t)0x080E0000)	/* Base @ of Bank1 Sector 7, 128 Kbytes */
#define ADDR_FLASH_BANK2_SECTOR_0		((uint32_t)0x08100000)	/* Base @ of Bank2 Sector 0, 128 Kbytes */
#define ADDR_FLASH_BANK2_SECTOR_1		((uint32_t)0x08120000)	/* Base @ of Bank2 Sector 1, 128 Kbytes */
#define ADDR_FLASH_BANK2_SECTOR_2		((uint32_t)0x08140000)	/* Base @ of Bank2 Sector 2, 128 Kbytes */
#define ADDR_FLASH_BANK2_SECTOR_3		((uint32_t)0x08160000)	/* Base @ of Bank2 Sector 3, 128 Kbytes */
#define ADDR_FLASH_BANK2_SECTOR_4		((uint32_t)0x08180000)	/* Base @ of Bank2 Sector 4, 128 Kbytes */
#define ADDR_FLASH_BANK2_SECTOR_5		((uint32_t)0x081A0000)	/* Base @ of Bank2 Sector 5, 128 Kbytes */
#define ADDR_FLASH_BANK2_SECTOR_6		((uint32_t)0x081C0000)	/* Base @ of Bank2 Sector 6, 128 Kbytes */
#define ADDR_FLASH_BANK2_SECTOR_7		((uint32_t)0x081E0000)	/* Base @ of Bank2 Sector 7, 128 Kbytes */

//用户接口
uint32_t JX_Flash_GetSampleNumber(void);
uint32_t JX_Flash_GetSampleAddr(uint8_t user_index,uint8_t subnumber);
void JX_Flash_SaveFingerSample(uint32_t uuid, uint32_t *data1,uint32_t *data2,uint32_t *data3);
void JX_Flash_DeletAllSamples(void);
//Table: Sector 0, UUID1,UUID2,UUID3.....（sample number）（CRC8）


/*************************************************************************************************/
//#define RFS
#define DATA_32                 ((uint32_t)0x12345678)
#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_4   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_8   /* End @ of user Flash area */
#define Sample_Size_Unit        ((uint32_t) 1024*2)

#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */


//内部接口
extern FLASH_EraseInitTypeDef EraseInitStruct;
extern uint32_t GetSector(uint32_t Address);
extern void EraseFlash(uint32_t start_addr, uint32_t end_addr);
extern void SaveSamplestoFlash(uint32_t *data1, uint32_t *data2, uint32_t *data3);
extern void Restore_Factory_Settings(void);
extern void Delet_All_Samples(void);
extern void UpdateTable(void);
extern uint8_t Find_Sample_Code(uint32_t sample_code);
extern uint8_t Delet_One_Table(uint8_t number);
extern uint8_t Delet_One_Sample(uint8_t number);
extern uint8_t Replace_One_Sample(uint32_t *data, uint8_t number, uint8_t sub_number);
extern uint8_t Send_One_Flash_Sample(uint8_t number);
extern void UpdateTable_Reco_Freq(uint8_t number);
extern void clrSampleTableBuf(void);
extern void FVD_FLASH_EnableRDP(void);
#endif


#ifndef FV_FLASH_H_LJS
#define FV_FLASH_H_LJS

#include "stm32h7xx_hal.h"
//用户存储空间分配方式
//ADDR_FLASH_BANK2_SECTOR_4		table
//ADDR_FLASH_BANK2_SECTOR_5		60个样本
//ADDR_FLASH_BANK2_SECTOR_6   60个样本
//ADDR_FLASH_BANK2_SECTOR_7		缓冲区
#define FLASH_DATA_VALIED_FLAG  0x56781234                  /* 数据有效标志 */
#define FLASH_USER_START_ADDR   ADDR_FLASH_BANK2_SECTOR_4   /* 用户数据首地址 */
#define FLASH_SECTOR_SIZE_H7		0x20000											/* 每个sector容量(字节) */
#define Sample_Size_Unit        ((uint32_t) 1024*2)					/* 每个样本所占字节数 */
#define FLASH_WAITETIME  				50000          							/* FLASH等待超时时间 */

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
void JX_Flash_Init(void);
//获取样本数量
//return 0-40
uint32_t JX_Flash_GetSampleNumber(void);
//获取第几根手指第几个样本的地址
//user_index(0-39)
//subnumber(0-2)
uint32_t JX_Flash_GetSampleAddr(uint8_t user_index, uint8_t subnumber);
//存储样本数据 特征码 三个样本地址
void JX_Flash_SaveFingerSample(uint32_t uuid, uint32_t *data1, uint32_t *data2, uint32_t *data3);
//删除所有样本
void JX_Flash_DeletAllSamples(void);
//user_index(0-39)
void JX_Flash_UpdateTable_Reco_Freq(uint8_t user_index);
void JX_Flash_GetSampleFrequency(uint8_t * table_freq);
void JX_Flash_UpdateData(uint8_t user_index,uint8_t subnumber,uint32_t *data);

/*************************************************************************************************/
//内部接口
//清除table缓冲区
static void JX_Flash_clrSampleTableBuf(void);
//crc校验
static uint8_t JX_Flash_CRC8(uint8_t *message, uint8_t len);
//获取扇区所在地址
static uint32_t JX_Flash_GetSector(uint32_t Address);
//擦除sector 起始地址 结束地址
static void JX_Flash_EraseFlash(uint32_t start_addr, uint32_t end_addr);
//一次写入32字节数据到指定位置 flash每个地址8bit数据
static void JX_Flash_WriteData(uint32_t FlashAddress, uint64_t DataAddress);
//从指定地址读出指定长度的数据(字)
 void JX_Flash_ReadData(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead);
//更新table
static void JX_Flash_UpdateTable(uint32_t uuid);
void JX_Flash_CopyDataToBufArea(uint8_t user_index, uint8_t subnumber);
void JX_Flash_CopyDataToDataArea(uint8_t user_index, uint8_t subnumber);
#endif


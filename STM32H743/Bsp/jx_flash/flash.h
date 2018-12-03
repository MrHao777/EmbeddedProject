#ifndef FV_FLASH_H_LJS
#define FV_FLASH_H_LJS

#include "stm32h7xx_hal.h"

#define FLASH_USER_START_ADDR   ADDR_FLASH_BANK2_SECTOR_5   /* �û������׵�ַ */
#define FLASH_SECTOR_SIZE_H7		0x20000											/* ÿ��sector����(�ֽ�) */
#define Sample_Size_Unit        ((uint32_t) 1024*2)					/* ÿ��������ռ�ֽ��� */
#define FLASH_WAITETIME  				50000          							/* FLASH�ȴ���ʱʱ�� */

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

//�û��ӿ�
//��ȡ��������
//return 0-40
uint32_t JX_Flash_GetSampleNumber(void);
//��ȡ�ڼ�����ָ�ڼ��������ĵ�ַ
//user_index(0-39)
//subnumber(0-2)
uint32_t JX_Flash_GetSampleAddr(uint8_t user_index,uint8_t subnumber);
//�洢�������� ������ ����������ַ
void JX_Flash_SaveFingerSample(uint32_t uuid, uint32_t *data1,uint32_t *data2,uint32_t *data3);
//ɾ����������
void JX_Flash_DeletAllSamples(void);
//Table: Sector 0, UUID1,UUID2,UUID3.....��sample number����CRC8��

/*************************************************************************************************/
//�ڲ��ӿ�
//���table������
static void JX_Flash_clrSampleTableBuf(void);
//crcУ��
static uint8_t JX_Flash_CRC8(uint8_t *message, uint8_t len);
//��ȡ�������ڵ�ַ
static uint32_t JX_Flash_GetSector(uint32_t Address);
//����sector ��ʼ��ַ ������ַ
static void JX_Flash_EraseFlash(uint32_t start_addr, uint32_t end_addr);
//һ��д��32�ֽ����ݵ�ָ��λ�� flashÿ����ַ8bit����
static void JX_Flash_WriteData(uint32_t FlashAddress, uint64_t DataAddress);
//��ָ����ַ����ָ�����ȵ�����(��)
static void JX_Flash_ReadData(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead);
//����table
static void JX_Flash_UpdateTable(uint32_t uuid);
#endif


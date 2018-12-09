#ifndef _flash_H
#define _flash_H

#include "stm32f1xx_hal.h"
/**********flash����˵��*********************
��������     : ADDR_FLASH_PAGE_61 0x0800F400
uuid_num     : 4�ֽ�
Verifmode    : 4�ֽ�
alertmode    : 4�ֽ�
AdmPassword  : 8�ֽ�
UserPassword : 8�ֽ�

uuid����     : ADDR_FLASH_PAGE_62 0x0800F800
uuid         : 40*4�ֽ�
*********************************************/

#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_61    			/* �û������׵�ַ */
#define FLASH_SECTOR_SIZE_F1		0x400 											/* ÿ��page����(�ֽ�) */
#define FLASH_WAITETIME  				100000          						/* FLASH�ȴ���ʱʱ�� */

#define ADDR_FLASH_PAGE_0		  ((uint32_t)0x08000000)	/* Base @ of Page   0, 1 Kbytes */
#define ADDR_FLASH_PAGE_1		  ((uint32_t)0x08000400)	/* Base @ of Page   1, 1 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0800F400)  /* Base @ of Page  61, 1 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0800F800)  /* Base @ of Page  62, 1 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0800FC00)  /* Base @ of Page  63, 1 Kbytes */
#define ADDR_FLASH_PAGE_125		((uint32_t)0x081A0400)	/* Base @ of Page 125, 1 Kbytes */
#define ADDR_FLASH_PAGE_126		((uint32_t)0x0801F800)	/* Base @ of Page 126, 1 Kbytes */
#define ADDR_FLASH_PAGE_127		((uint32_t)0x0801FC00)	/* Base @ of Page 127, 1 Kbytes */

uint32_t JX_GetOpenDoorMode(void); 
uint32_t JX_GetAlertMode(void);
//ƥ�䷵��0 ���򷵻�-1
int32_t JX_CheckAdmPassword(int8_t* password_pointer);
//ƥ�䷵��0 ���򷵻�-1
int32_t JX_CheckUserPassword(int8_t* password_pointer);
//ƥ�䷵��0 ���򷵻�-1
int32_t JX_CheckUUID(uint32_t uuid); 
void JX_DeletUUID(void); 
void JX_SetOpenDoorMode(int32_t mode);
void JX_SetAlertMode(int32_t mode);
void JX_SetAdmPassword(int8_t* password_pointer); 
void JX_SetUserPassword(int8_t* password_pointer); 
void JX_SaveUUID(uint32_t uuid); 

//�ڲ��ӿ�
static void JX_Flash_clrSampleTableBuf(void);
//����page  PageNumber(0-63)
static void JX_Flash_EraseFlash(uint8_t PageNumber);
static void JX_Flash_WriteData(uint32_t FlashAddress, uint32_t *DataAddress, uint32_t WordNumber);
static void JX_Flash_ReadData(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead);

#endif

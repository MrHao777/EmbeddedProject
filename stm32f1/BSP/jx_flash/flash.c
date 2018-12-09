#include "flash.h"
#include "string.h"

static uint32_t Flash_Buffer[40];

uint32_t JX_GetOpenDoorMode(void)
{
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 2);
	
	return Flash_Buffer[1];
}

uint32_t JX_GetAlertMode(void)
{
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 3);
	
	return Flash_Buffer[2];
}

int32_t JX_CheckAdmPassword(int8_t* password_pointer)
{
	uint8_t compare_state;
	
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 5);
	compare_state = strncmp ( (char*)password_pointer,(char*)Flash_Buffer + 12, 8 );
	
	if(compare_state)
		return -1;
	else
		return 0;
}

int32_t JX_CheckUserPassword(int8_t* password_pointer)
{
	uint8_t compare_state;
	
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
	compare_state = strncmp ( (char*)password_pointer,(char*)Flash_Buffer + 20, 8 );
	
	if(compare_state)
		return -1;
	else
		return 0;
}

int32_t JX_CheckUUID(uint32_t uuid)
{
	uint32_t uuid_num;
	uint8_t i;
	
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 1);
	uuid_num = Flash_Buffer[0];
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_F1, Flash_Buffer, uuid_num);
	for(i=0; i<uuid_num; i++)
	{
		if(Flash_Buffer[i] == uuid)
			return 0;
	}
	
	return -1;
}

void JX_DeletUUID(void)
{	
	//清除uuid
	JX_Flash_clrSampleTableBuf();
	JX_Flash_EraseFlash(62);
	JX_Flash_WriteData(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_F1, Flash_Buffer, 40);	
	
	//更新num
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
	Flash_Buffer[0] = 0;
	JX_Flash_EraseFlash(61);
	JX_Flash_WriteData(FLASH_USER_START_ADDR, Flash_Buffer, 7);	
}

void JX_SetOpenDoorMode(int32_t mode)
{
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
	Flash_Buffer[1] = mode;
	JX_Flash_EraseFlash(61);
	JX_Flash_WriteData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
}

void JX_SetAlertMode(int32_t mode)
{
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
	Flash_Buffer[2] = mode;
	JX_Flash_EraseFlash(61);
	JX_Flash_WriteData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
}

void JX_SetAdmPassword(int8_t* password_pointer)
{
	uint8_t i;
	
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
	for(i=0; i<8; i++)
		*((int8_t *)Flash_Buffer + 12 + i) = *(password_pointer + i);
	JX_Flash_EraseFlash(61);
	JX_Flash_WriteData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
}
void JX_SetUserPassword(int8_t* password_pointer)
{
	uint8_t i;
	
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
	for(i=0; i<8; i++)
		*((int8_t *)Flash_Buffer + 20 + i) = *(password_pointer + i);
	JX_Flash_EraseFlash(61);
	JX_Flash_WriteData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
}

void JX_SaveUUID(uint32_t uuid)
{
	uint32_t uuid_num;
	
	
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
	uuid_num = Flash_Buffer[0];
	//更新uuid
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_F1, Flash_Buffer, 40);
	Flash_Buffer[uuid_num] = uuid;
	JX_Flash_EraseFlash(62);
	JX_Flash_WriteData(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_F1, Flash_Buffer, 40);
	
	//更新num
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
	Flash_Buffer[0]++;
	JX_Flash_EraseFlash(61);
	JX_Flash_WriteData(FLASH_USER_START_ADDR, Flash_Buffer, 7);
}

/************************************************/
static void JX_Flash_clrSampleTableBuf(void) 
{
	for(int i = 0; i < 40; i++) 
		Flash_Buffer[i] = 0;
}

static void JX_Flash_EraseFlash(uint8_t PageNumber)
{ 
	FLASH_EraseInitTypeDef 		FlashEraseInit;
	uint32_t SectorError = 0;
	
	if(HAL_FLASH_Unlock()!=HAL_OK) //解锁
	{
			_Error_Handler(__FILE__, __LINE__);
	}
	FlashEraseInit.Banks = FLASH_BANK_1;
	FlashEraseInit.NbPages = 1;
	FlashEraseInit.PageAddress = ADDR_FLASH_PAGE_0 + (uint32_t)(PageNumber * FLASH_SECTOR_SIZE_F1);
	FlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
	{
			_Error_Handler(__FILE__, __LINE__);
	}
	if(FLASH_WaitForLastOperation(FLASH_WAITETIME))
	{
			_Error_Handler(__FILE__, __LINE__);
	}
	if(HAL_FLASH_Lock()!=HAL_OK) 
	{
			_Error_Handler(__FILE__, __LINE__);
	}
}	

static void JX_Flash_WriteData(uint32_t FlashAddress, uint32_t *DataAddress, uint32_t WordNumber)
{
	uint32_t TempFlashAddress = FlashAddress;
	uint32_t *TempDataAddress = DataAddress;
	uint32_t i ;
	
	if(HAL_FLASH_Unlock()!=HAL_OK) 
	{
			_Error_Handler(__FILE__, __LINE__);
	}
	for(i=0; i<WordNumber; i++)
	{
		if(FLASH_WaitForLastOperation(FLASH_WAITETIME) != HAL_OK)
		{
				_Error_Handler(__FILE__, __LINE__);
		}
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, TempFlashAddress, (uint64_t)*TempDataAddress) != HAL_OK)
		{
				
				_Error_Handler(__FILE__, __LINE__);
		}
		TempFlashAddress += 4;
		TempDataAddress ++;
	}
	if(HAL_FLASH_Lock()!=HAL_OK) 
	{
			_Error_Handler(__FILE__, __LINE__);
	}
}

static void JX_Flash_ReadData(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead)
{
	uint32_t i;
	
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i] = *(__IO uint32_t *)(ReadAddr);
		ReadAddr += 4;	
	}
}


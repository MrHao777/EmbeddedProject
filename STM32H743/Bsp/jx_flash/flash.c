#include "flash.h"

//table������(������ : 40*4Byte		�������� : 4Byte		CRC : 4byte) 
static uint32_t Sample_Table_Buffer[48] ;


uint32_t JX_Flash_GetSampleNumber(void)
{
	uint32_t simple_number;
	
	JX_Flash_ReadData(FLASH_USER_START_ADDR + 40 * 4 , &simple_number, 1);
	
	return simple_number;
}

uint32_t JX_Flash_GetSampleAddr(uint8_t user_index,uint8_t subnumber)
{
	uint32_t addr;
	
	if(user_index < 20)
	{
		addr = FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7;
	}
	else
	{
		addr = FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 2;
	}
	addr += 3 * Sample_Size_Unit * user_index + Sample_Size_Unit * subnumber;
	
	return addr;
}

void JX_Flash_SaveFingerSample(uint32_t uuid, uint32_t *data1,uint32_t *data2,uint32_t *data3)
{
	uint32_t addr, i;
	uint8_t SampleNumber;
	
	SampleNumber = JX_Flash_GetSampleNumber();
	//д���һ������
	addr = JX_Flash_GetSampleAddr(SampleNumber, 0);
	for(i=0; i<Sample_Size_Unit/4; i += 8)
		JX_Flash_WriteData(addr + i*4, (uint64_t)(data1 + i));
	//д��ڶ�������
	addr = JX_Flash_GetSampleAddr(SampleNumber, 1);
	for(i=0; i<Sample_Size_Unit/4; i += 8)
		JX_Flash_WriteData(addr + i*4, (uint64_t)(data2 + i));
	//д�����������
	addr = JX_Flash_GetSampleAddr(SampleNumber, 2);
	for(i=0; i<Sample_Size_Unit/4; i += 8)
		JX_Flash_WriteData(addr + i*4, (uint64_t)(data3 + i));
	//����table
	JX_Flash_UpdateTable(uuid);
}

void JX_Flash_DeletAllSamples(void)
{
	uint8_t i;
	
	//��������sector
	JX_Flash_EraseFlash(FLASH_USER_START_ADDR, FLASH_USER_START_ADDR);
	JX_Flash_EraseFlash(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7, FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7);
	JX_Flash_EraseFlash(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 2, FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 2);
	//�½�table
	JX_Flash_clrSampleTableBuf();
	for(i=0; i<48; i += 8)
		JX_Flash_WriteData(FLASH_USER_START_ADDR + i*4, (uint64_t)(Sample_Table_Buffer + i));

}

/*****************************************************************/
static void JX_Flash_clrSampleTableBuf(void) 
{
	for(int i = 0; i < 48; i++) 
		Sample_Table_Buffer[i] = 0;
}

static uint8_t JX_Flash_CRC8(uint8_t *message, uint8_t len)
{
    uint8_t crc;
    uint8_t i;
    crc = 0;
    while(len--)
    {
        crc ^= *message++;
        for(i = 0;i < 8;i++)
        {
            if(crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8c;
            }
                else crc >>= 1;
        }
    }
    return crc;
}

//��ȡĳ��ַ����flash����,������BANK2
uint32_t JX_Flash_GetSector(uint32_t Address)
{
  uint32_t sector = 0;
	
  if(Address < ADDR_FLASH_BANK2_SECTOR_0)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	else if((Address < ADDR_FLASH_BANK2_SECTOR_1) && (Address >= ADDR_FLASH_BANK2_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_BANK2_SECTOR_2) && (Address >= ADDR_FLASH_BANK2_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_BANK2_SECTOR_3) && (Address >= ADDR_FLASH_BANK2_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_BANK2_SECTOR_4) && (Address >= ADDR_FLASH_BANK2_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_BANK2_SECTOR_5) && (Address >= ADDR_FLASH_BANK2_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_BANK2_SECTOR_6) && (Address >= ADDR_FLASH_BANK2_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_BANK2_SECTOR_7) && (Address >= ADDR_FLASH_BANK2_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else 
  {
    sector = FLASH_SECTOR_7;  
  }

  return sector;
}

static void JX_Flash_EraseFlash(uint32_t start_addr, uint32_t end_addr)
{ 
	FLASH_EraseInitTypeDef 		FlashEraseInit;
	uint32_t FirstSector = 0, NbOfSectors = 0;
	uint32_t SectorError = 0;
	
	FirstSector = JX_Flash_GetSector(start_addr);
	NbOfSectors = JX_Flash_GetSector(end_addr) - FirstSector + 1;
	
	if(HAL_FLASH_Unlock()!=HAL_OK) //����
	{
			_Error_Handler(__FILE__, __LINE__);
	}
	FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //�������ͣ��������� 
	FlashEraseInit.Sector= FirstSector;   									//Ҫ����������
	FlashEraseInit.Banks=FLASH_BANK_2;											//����BANK1
	FlashEraseInit.NbSectors= NbOfSectors;                  //����������
	FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //��ѹ��Χ
	if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
	{
			_Error_Handler(__FILE__, __LINE__);
	}
//	SCB_CleanInvalidateDCache();                            			//�����Ч��D-Cache
	FLASH_WaitForLastOperation(FLASH_WAITETIME,FLASH_BANK_2);    	//�ȴ��ϴβ������
	if(HAL_FLASH_Lock()!=HAL_OK) 
	{
			_Error_Handler(__FILE__, __LINE__);
	}
}

static void JX_Flash_WriteData(uint32_t FlashAddress, uint64_t DataAddress)
{
	if(HAL_FLASH_Unlock()!=HAL_OK) //����
	{
			_Error_Handler(__FILE__, __LINE__);
	}
	if(FLASH_WaitForLastOperation(FLASH_WAITETIME,FLASH_BANK_2))
	{
			_Error_Handler(__FILE__, __LINE__);
	}
	if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, FlashAddress, DataAddress) != HAL_OK)
	{
			_Error_Handler(__FILE__, __LINE__);
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

static void JX_Flash_UpdateTable(uint32_t uuid)
{
	uint8_t SampleNumber;
	uint8_t i;
	uint32_t crc;
	
	SampleNumber = JX_Flash_GetSampleNumber();
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Sample_Table_Buffer, SampleNumber);
	Sample_Table_Buffer[SampleNumber] = uuid;
	Sample_Table_Buffer[40] += 1;
	crc = JX_Flash_CRC8((uint8_t *)Sample_Table_Buffer, 41 * 4);
	Sample_Table_Buffer[41] = crc;
	JX_Flash_EraseFlash(FLASH_USER_START_ADDR, FLASH_USER_START_ADDR);
	for(i=0; i<48; i += 8)
		JX_Flash_WriteData(FLASH_USER_START_ADDR + i*4, (uint64_t)(Sample_Table_Buffer + i));
}
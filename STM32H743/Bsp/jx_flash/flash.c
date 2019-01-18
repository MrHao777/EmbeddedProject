#include "flash.h" 

//                        0 -39                 40                41 - 50            51              52
//table缓冲区(特征码 : 40*4Byte		样本总数 : 4Byte		频率纪录 : 40*1Byte		CRC : 4byte  数据区有效标志) 
static uint32_t Sample_Table_Buffer[56] ;
//16
void JX_Flash_Init(void)
{
	uint8_t i;
	
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Sample_Table_Buffer, 53);
	if(Sample_Table_Buffer[52] != FLASH_DATA_VALIED_FLAG)
		JX_Flash_DeletAllSamples();
}

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
	else if(user_index < 40)
	{
		addr = FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 2;
	}
	else
	{
		addr = FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 3;
	}
	
	addr += 3 * Sample_Size_Unit * user_index + Sample_Size_Unit * subnumber;
	
	return addr;
}

void JX_Flash_SaveFingerSample(uint32_t uuid, uint32_t *data1,uint32_t *data2,uint32_t *data3)
{
	uint32_t addr, i;
	uint8_t SampleNumber;
	
	SampleNumber = JX_Flash_GetSampleNumber();
	//写入第一个样本
	addr = JX_Flash_GetSampleAddr(SampleNumber, 0);
	for(i=0; i<Sample_Size_Unit/4; i += 8)
		JX_Flash_WriteData(addr + i*4, (uint64_t)(data1 + i));
	//写入第二个样本
	addr = JX_Flash_GetSampleAddr(SampleNumber, 1);
	for(i=0; i<Sample_Size_Unit/4; i += 8)
		JX_Flash_WriteData(addr + i*4, (uint64_t)(data2 + i));
	//写入第三个样本
	addr = JX_Flash_GetSampleAddr(SampleNumber, 2);
	for(i=0; i<Sample_Size_Unit/4; i += 8)
		JX_Flash_WriteData(addr + i*4, (uint64_t)(data3 + i));
	//更新table
	JX_Flash_UpdateTable(uuid);
}

//                        0 -39                 40                41 - 50            51              52
//table缓冲区(特征码 : 40*4Byte		样本总数 : 4Byte		频率纪录 : 40*1Byte		CRC : 4byte  数据区有效标志) 
//16
void JX_Flash_UpdateData(uint8_t user_index,uint8_t subnumber,uint32_t *data)
{
	uint32_t SampleNumber;
	uint8_t i, j;
	uint32_t k;
	uint32_t source_addr, dist_addr;

	SampleNumber = JX_Flash_GetSampleNumber();
	JX_Flash_EraseFlash(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 3, FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 3);
  for(i=0; i< SampleNumber; i++)
	{
		for(j=0; j<3; j++)
		{
			if(i==user_index && j==subnumber)
			{				
				if(user_index < 20)
					dist_addr = JX_Flash_GetSampleAddr(user_index + 40, subnumber);
				else
					dist_addr = JX_Flash_GetSampleAddr(user_index + 20, subnumber);
				for(k=0; k<Sample_Size_Unit/4; k += 8)
					JX_Flash_WriteData(dist_addr + k*4, (uint64_t)(data + k));
			}
			else
			{
				JX_Flash_CopyDataToBufArea(i, j);
			}
		}
	}
	if(user_index<20)
		JX_Flash_EraseFlash(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 1, FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 3);
	else
		JX_Flash_EraseFlash(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 2, FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 3);
	for(i=0; i<SampleNumber; i++)
	{
		for(j=0; j<3; j++)
		{
			JX_Flash_CopyDataToDataArea(i, j);
		}
	}
}

void JX_Flash_DeletAllSamples(void)
{
	uint8_t i;
	
	//擦除所有sector
	JX_Flash_EraseFlash(FLASH_USER_START_ADDR, FLASH_USER_START_ADDR);
	JX_Flash_EraseFlash(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7, FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7);
	JX_Flash_EraseFlash(FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 2, FLASH_USER_START_ADDR + FLASH_SECTOR_SIZE_H7 * 2);
	//新建table
	JX_Flash_clrSampleTableBuf();
	Sample_Table_Buffer[52] = FLASH_DATA_VALIED_FLAG;
	for(i=0; i<56; i += 8)
		JX_Flash_WriteData(FLASH_USER_START_ADDR + i*4, (uint64_t)(Sample_Table_Buffer + i));
}

void JX_Flash_UpdateTable_Reco_Freq(uint8_t user_index)
{
	uint32_t crc;
	uint8_t i;
	
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Sample_Table_Buffer, 52);
	*((uint8_t*)Sample_Table_Buffer + 41 * 4 + user_index) += 1;
	crc = JX_Flash_CRC8((uint8_t *)Sample_Table_Buffer, 51 * 4);
	Sample_Table_Buffer[51] = crc;
	Sample_Table_Buffer[52] = FLASH_DATA_VALIED_FLAG;
	JX_Flash_EraseFlash(FLASH_USER_START_ADDR, FLASH_USER_START_ADDR);
	for(i=0; i<56; i += 8)
		JX_Flash_WriteData(FLASH_USER_START_ADDR + i*4, (uint64_t)(Sample_Table_Buffer + i));
}

void JX_Flash_GetSampleFrequency(uint8_t * table_freq)
{
	uint8_t i;
	
	JX_Flash_clrSampleTableBuf();
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Sample_Table_Buffer, 52);
	for(i=0; i<40; i++)
		table_freq[i] = *((uint8_t*)Sample_Table_Buffer + 41 * 4 + i);
}

/*****************************************************************/
static void JX_Flash_clrSampleTableBuf(void) 
{
	uint32_t crc;
	
	for(int i = 0; i < 56; i++) 
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

//获取某地址所在flash扇区,仅用于BANK2
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
	
	if(HAL_FLASH_Unlock()!=HAL_OK) //解锁
	{
			_Error_Handler(__FILE__, __LINE__);
	}
	FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除 
	FlashEraseInit.Sector= FirstSector;   									//要擦除的扇区
	FlashEraseInit.Banks=FLASH_BANK_2;											//操作BANK1
	FlashEraseInit.NbSectors= NbOfSectors;                  //擦除扇区数
	FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围
	if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
	{
			_Error_Handler(__FILE__, __LINE__);
	}
//	SCB_CleanInvalidateDCache();                            			//清除无效的D-Cache
	FLASH_WaitForLastOperation(FLASH_WAITETIME,FLASH_BANK_2);    	//等待上次操作完成
	if(HAL_FLASH_Lock()!=HAL_OK) 
	{
			_Error_Handler(__FILE__, __LINE__);
	}
}

static void JX_Flash_WriteData(uint32_t FlashAddress, uint64_t DataAddress)
{
	if(HAL_FLASH_Unlock()!=HAL_OK) //解锁
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

 void JX_Flash_ReadData(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead)
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
	JX_Flash_ReadData(FLASH_USER_START_ADDR, Sample_Table_Buffer, 52);
	Sample_Table_Buffer[SampleNumber] = uuid;
	Sample_Table_Buffer[40] += 1;
	crc = JX_Flash_CRC8((uint8_t *)Sample_Table_Buffer, 51 * 4);
	Sample_Table_Buffer[51] = crc;
	Sample_Table_Buffer[52] = FLASH_DATA_VALIED_FLAG;
	JX_Flash_EraseFlash(FLASH_USER_START_ADDR, FLASH_USER_START_ADDR);
	for(i=0; i<56; i += 8)
		JX_Flash_WriteData(FLASH_USER_START_ADDR + i*4, (uint64_t)(Sample_Table_Buffer + i));
}

void JX_Flash_CopyDataToBufArea(uint8_t user_index, uint8_t subnumber)
{
	uint8_t i;
	uint32_t source_addr, dist_addr;
	
	source_addr = JX_Flash_GetSampleAddr(user_index, subnumber);
	if(user_index < 20)
		dist_addr = JX_Flash_GetSampleAddr(user_index + 40, subnumber);
	else
		dist_addr = JX_Flash_GetSampleAddr(user_index + 20, subnumber);
	for(i=0; i<64; i++)
	{
		JX_Flash_ReadData(source_addr + i * 32, Sample_Table_Buffer, 8);
		JX_Flash_WriteData(dist_addr + i * 32, (uint64_t)Sample_Table_Buffer);
	}
}

void JX_Flash_CopyDataToDataArea(uint8_t user_index, uint8_t subnumber)
{
	uint8_t i;
	uint32_t source_addr, dist_addr;

	if(user_index < 20)
		source_addr = JX_Flash_GetSampleAddr(user_index + 40, subnumber);
	else
		source_addr = JX_Flash_GetSampleAddr(user_index + 20, subnumber);
	dist_addr = JX_Flash_GetSampleAddr(user_index, subnumber);
	for(i=0; i<64; i++)
	{
		JX_Flash_ReadData(source_addr + i * 32, Sample_Table_Buffer, 8);
		JX_Flash_WriteData(dist_addr + i * 32, (uint64_t)Sample_Table_Buffer);
	}	
}

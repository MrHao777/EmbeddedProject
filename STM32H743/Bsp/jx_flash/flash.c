#include "flash.h"

extern UART_HandleTypeDef huart1;
extern RNG_HandleTypeDef hrng;
extern uint32_t UUID_Reg;
extern uint8_t Error_Message[256];

const uint8_t Flash_Table_Check[2] = {0x12, 0x05};
uint8_t Sample_Table[204] __attribute__((aligned(4)));
void clrSampleTableBuf(void) {for(int i = 0; i < 204; i++) Sample_Table[i] = 0;}

uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
  else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11) */
  {
    sector = FLASH_SECTOR_11;
  }

  return sector;
}

void EraseFlash(uint32_t start_addr, uint32_t end_addr)
{
	uint32_t FirstSector = 0, NbOfSectors = 0;
	uint32_t SectorError = 0;
	FirstSector = GetSector(start_addr);
	NbOfSectors = GetSector(end_addr) - FirstSector + 1;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FirstSector;
  EraseInitStruct.NbSectors = NbOfSectors;
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  { 
		sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
    Error_Handler();
  }
	__HAL_FLASH_DATA_CACHE_DISABLE();
  __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

  __HAL_FLASH_DATA_CACHE_RESET();
  __HAL_FLASH_INSTRUCTION_CACHE_RESET();

  __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
  __HAL_FLASH_DATA_CACHE_ENABLE();
}

void SaveSamplestoFlash(uint32_t *data1, uint32_t *data2, uint32_t *data3)
{
	uint32_t Address = ADDR_FLASH_SECTOR_4;
	uint8_t SampleNumber = *(__IO uint8_t*)Address;
	if(SampleNumber < 20)
		Address = ADDR_FLASH_SECTOR_5 + Sample_Size_Unit*3*SampleNumber;
	else
		Address = ADDR_FLASH_SECTOR_6 + Sample_Size_Unit*3*(SampleNumber-20);
	HAL_FLASH_Unlock(); 
	uint32_t data32;
	for(uint32_t i = 0; i < Sample_Size_Unit/4; i++)
	{
		data32 = data1[i];
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, data32) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    { 
			sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
      Error_Handler();
    }
	}
	
	for(uint32_t i = 0; i < Sample_Size_Unit/4; i++)
	{
		data32 = data2[i];
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, data32) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    { 
			sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
      Error_Handler();
    }
	}
	
	for(uint32_t i = 0; i < Sample_Size_Unit/4; i++)
	{
		data32 = data3[i];
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, data32) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    { 
			sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
      Error_Handler();
    }
	}
	HAL_FLASH_Lock();	
	UpdateTable();
}

void UpdateTable(void)
{
	clrSampleTableBuf();
	uint32_t Address = ADDR_FLASH_SECTOR_4;
	uint32_t *Samp_address = (uint32_t *)Sample_Table;
	for(uint8_t i = 0; i < 51; i++)
	{
		Samp_address[i] = *(__IO uint32_t*)Address;
		Address = Address + 4;
	}
	Sample_Table[0] = Sample_Table[0] + 1;
	uint8_t N_j = Sample_Table[0];
	
	uint32_t RandomNumber1 = 0;
	uint32_t RandomNumber2 = 0;
	while(RandomNumber1 == RandomNumber2)
	{
		HAL_RNG_GenerateRandomNumber(&hrng,&RandomNumber1);
		HAL_RNG_GenerateRandomNumber(&hrng,&RandomNumber2);	
	}
	UUID_Reg = RandomNumber2;
	uint8_t *rndata =(uint8_t *)&RandomNumber2;
	for(uint8_t i = 0; i < 4; i++)
	{
		uint8_t p_i = 4*(N_j-1) + 2 + i;
		Sample_Table[p_i] = rndata[i];
	}	
	HAL_FLASH_Unlock(); 
	EraseFlash(ADDR_FLASH_SECTOR_4, ADDR_FLASH_SECTOR_4);
	Address = ADDR_FLASH_SECTOR_4;
	for(uint8_t i = 0; i < 51; i++)
	{
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Samp_address[i]) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    { 
			sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
      Error_Handler();
    }
	}
	HAL_FLASH_Lock();	
}

void UpdateTable_Reco_Freq(uint8_t number)
{
	if(number < 1 || number > 40)
		return;
	clrSampleTableBuf();
	uint32_t Address = ADDR_FLASH_SECTOR_4;
	uint32_t *Samp_address = (uint32_t *)Sample_Table;
	for(uint8_t i = 0; i < 51; i++)
	{
		Samp_address[i] = *(__IO uint32_t*)Address;
		Address = Address + 4;
	}
	if(Sample_Table[161 + number] < 254)
		Sample_Table[161 + number] = Sample_Table[161 + number] + 1;
	else
	{
		for(uint8_t i = 162; i < 202; i++)
			Sample_Table[i] = 0;
		Sample_Table[161 + number] = 1;
	}
	HAL_FLASH_Unlock(); 
	EraseFlash(ADDR_FLASH_SECTOR_4, ADDR_FLASH_SECTOR_4);
	Address = ADDR_FLASH_SECTOR_4;
	for(uint8_t i = 0; i < 51; i++)
	{
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Samp_address[i]) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    { 
			sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
      Error_Handler();
    }
	}
	HAL_FLASH_Lock();	
}

void Delet_All_Samples(void)
{
	HAL_FLASH_Unlock(); 
	clrSampleTableBuf();
	uint32_t Address = ADDR_FLASH_SECTOR_4;
	Sample_Table[1] = *(__IO uint8_t*)(Address + 1);
	Sample_Table[202] = *(__IO uint8_t*)(Address + 202);
	Sample_Table[203] = *(__IO uint8_t*)(Address + 203);
	
	uint32_t *Samp_address = (uint32_t *)Sample_Table;
	EraseFlash(ADDR_FLASH_SECTOR_4, ADDR_FLASH_SECTOR_7);

	for(uint8_t i = 0; i < 51; i++)
	{
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Samp_address[i]) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    { 
			sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
      Error_Handler();
    }
	}
	HAL_FLASH_Lock();	
}

void Restore_Factory_Settings(void)
{
	HAL_FLASH_Unlock(); 
	clrSampleTableBuf();
	uint32_t Address = ADDR_FLASH_SECTOR_4;
	Sample_Table[202] = Flash_Table_Check[0];
	Sample_Table[203] = Flash_Table_Check[1];
	uint32_t *Samp_address = (uint32_t *)Sample_Table;
	EraseFlash(ADDR_FLASH_SECTOR_4, ADDR_FLASH_SECTOR_7);
	
	for(uint8_t i = 0; i < 51; i++)
	{
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Samp_address[i]) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    { 
			sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
      Error_Handler();
    }
	}
	HAL_FLASH_Lock();	
}

uint8_t Find_Sample_Code(uint32_t sample_code)
{
	uint32_t Address = ADDR_FLASH_SECTOR_4;
	uint8_t S_Number = *(__IO uint8_t*)Address;
	uint32_t data = 0;
	uint8_t i_number = 0;
	uint8_t count = 0;

	for(uint8_t i = 0; i < S_Number; i++)
	{
		data = *(__IO uint32_t*)(Address + 4*i + 2);
		if(data == sample_code) 
		{
			i_number = i;
			break;
		}
		count = count + 1;
	}
	if(count < S_Number)
		return (i_number + 1);
	else
		return 0;
}

uint8_t Delet_One_Table(uint8_t number)
{
	clrSampleTableBuf();
	uint32_t *Samp_address = (uint32_t *)Sample_Table;
	uint32_t Address = ADDR_FLASH_SECTOR_4;
	uint8_t S_number = *(__IO uint8_t*)Address;
	uint8_t d1 = 0;
	uint8_t d2 = 0;
	d1 = 2 + (number - 1)*4;
	d2 = 162 + (number - 1);
	if(number == 0 || number > S_number)
		return 1;
	for(uint8_t i = 0; i < 162 - 4; i++)
	{
		if(i == d1)
		{
			Address = Address + 4;
		}
		Sample_Table[i] = *(__IO uint8_t*)Address;
		Address = Address + 1;
	}
	
	Address = ADDR_FLASH_SECTOR_4 + 162;
	for(uint8_t i = 162; i < 202 -1; i++)
	{
		if(i == d2)
		{
			Address = Address + 1;
		}
		Sample_Table[i] = *(__IO uint8_t*)Address;
		Address = Address + 1;
	}
	Address = ADDR_FLASH_SECTOR_4 + 202;
	for(uint8_t i = 202; i < 204; i++)
	{
		Sample_Table[i] = *(__IO uint8_t*)Address;
		Address = Address + 1;
	}
	Sample_Table[0] = Sample_Table[0] - 1;
	
	HAL_FLASH_Unlock(); 
	EraseFlash(ADDR_FLASH_SECTOR_4, ADDR_FLASH_SECTOR_4);
	Address = ADDR_FLASH_SECTOR_4;
	
	for(uint8_t i = 0; i < 204/4; i++)
	{
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Samp_address[i]) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    { 
			sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
      Error_Handler();
    }
	}
	HAL_FLASH_Lock();	
	return 0;
}

uint8_t Delet_One_Sample(uint8_t number)
{
	uint32_t Address = ADDR_FLASH_SECTOR_4;
	uint8_t SampleNumber = *(__IO uint8_t*)Address;
	if(number == 0 || number > SampleNumber)
		return 1;
	
	HAL_FLASH_Unlock();
	EraseFlash(ADDR_FLASH_SECTOR_7, ADDR_FLASH_SECTOR_7);
	uint32_t i_end;
	uint32_t i_flag;
	uint32_t temp_data = 0;
	uint32_t Address_S;
	uint32_t Address_T;
	if(number > 20)
	{
		i_end = (SampleNumber - 20 -1)*3*Sample_Size_Unit/4;
		i_flag = (number - 20 -1)*3*Sample_Size_Unit/4;
		Address_S = ADDR_FLASH_SECTOR_6;
		Address_T = ADDR_FLASH_SECTOR_7;
		//copy data from sector6 to sector7 
		for(uint32_t i = 0; i < i_end; i++)
		{
			if(i == i_flag)
			{
				Address_S = Address_S + 3*Sample_Size_Unit;
			}
			temp_data = *(__IO uint32_t*)Address_S;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_T, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
		//copy data from sector7 back to sector6
		Address_S = ADDR_FLASH_SECTOR_6;
		Address_T = ADDR_FLASH_SECTOR_7;
		EraseFlash(ADDR_FLASH_SECTOR_6, ADDR_FLASH_SECTOR_6);
		for(uint32_t i = 0; i < i_end; i++)
		{
			temp_data = *(__IO uint32_t*)Address_T;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_S, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
	}
	else if(number < 21 && SampleNumber < 21)
	{
		Address_S = ADDR_FLASH_SECTOR_5;
		Address_T = ADDR_FLASH_SECTOR_7;
		i_end = (SampleNumber -1)*3*Sample_Size_Unit/4;
		i_flag = (number -1)*3*Sample_Size_Unit/4;
		//copy data from sector5 to sector7 
		for(uint32_t i = 0; i < i_end; i++)
		{
			if(i == i_flag)
			{
				Address_S = Address_S + 3*Sample_Size_Unit;
			}
			temp_data = *(__IO uint32_t*)Address_S;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_T, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
		//copy data from sector7 back to sector6
		Address_S = ADDR_FLASH_SECTOR_5;
		Address_T = ADDR_FLASH_SECTOR_7;
		EraseFlash(ADDR_FLASH_SECTOR_5, ADDR_FLASH_SECTOR_5);
		for(uint32_t i = 0; i < i_end; i++)
		{
			temp_data = *(__IO uint32_t*)Address_T;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_S, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
	}
	else if(number < 21 && SampleNumber > 20)
	{
		Address_S = ADDR_FLASH_SECTOR_5;
		Address_T = ADDR_FLASH_SECTOR_7;
		i_end = (20 -1)*3*Sample_Size_Unit/4;
		i_flag = (number -1)*3*Sample_Size_Unit/4;
		//copy data from sector5 to sector7 
		for(uint32_t i = 0; i < i_end; i++)
		{
			if(i == i_flag)
			{
				Address_S = Address_S + 3*Sample_Size_Unit;
			}
			temp_data = *(__IO uint32_t*)Address_S;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_T, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
		//copy data from sector7 back to sector5
		Address_S = ADDR_FLASH_SECTOR_5;
		Address_T = ADDR_FLASH_SECTOR_7;
		EraseFlash(ADDR_FLASH_SECTOR_5, ADDR_FLASH_SECTOR_5);
		for(uint32_t i = 0; i < i_end; i++)
		{
			temp_data = *(__IO uint32_t*)Address_T;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_S, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
		//copy the first sample from sector 6 to the end of sector 5
		Address_S = ADDR_FLASH_SECTOR_5 + (20 -1)*3*Sample_Size_Unit;
		Address_T = ADDR_FLASH_SECTOR_6;
		for(uint32_t i = 0; i < 3*Sample_Size_Unit/4; i++)
		{
			temp_data = *(__IO uint32_t*)Address_T;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_S, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
		
		EraseFlash(ADDR_FLASH_SECTOR_7, ADDR_FLASH_SECTOR_7);
		i_end = (SampleNumber - 20 -1)*3*Sample_Size_Unit/4;
		i_flag = 0;
		Address_S = ADDR_FLASH_SECTOR_6;
		Address_T = ADDR_FLASH_SECTOR_7;
		//copy data from sector6 to sector7 
		for(uint32_t i = 0; i < i_end; i++)
		{
			if(i == i_flag)
			{
				Address_S = Address_S + 3*Sample_Size_Unit;
			}
			temp_data = *(__IO uint32_t*)Address_S;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_T, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
		//copy data from sector7 back to sector6
		Address_S = ADDR_FLASH_SECTOR_6;
		Address_T = ADDR_FLASH_SECTOR_7;
		EraseFlash(ADDR_FLASH_SECTOR_6, ADDR_FLASH_SECTOR_6);
		for(uint32_t i = 0; i < i_end; i++)
		{
			temp_data = *(__IO uint32_t*)Address_T;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_S, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
	}
	EraseFlash(ADDR_FLASH_SECTOR_7, ADDR_FLASH_SECTOR_7);
	HAL_FLASH_Lock();	
	return 0;
}

uint8_t Replace_One_Sample(uint32_t *data, uint8_t number, uint8_t sub_number)
{
	uint32_t Address = ADDR_FLASH_SECTOR_4;
	uint8_t SampleNumber = *(__IO uint8_t*)Address;
	if(number > SampleNumber || sub_number > 2 || number < 1)
		return 1;
	
	HAL_FLASH_Unlock();
	EraseFlash(ADDR_FLASH_SECTOR_7, ADDR_FLASH_SECTOR_7);
	uint32_t i_end;
	uint32_t i_flag;
	uint32_t temp_data = 0;
	uint32_t Address_S;
	uint32_t Address_T;
	
	if(number < 21)
	{
		if(SampleNumber < 21)
			i_end = SampleNumber*3*Sample_Size_Unit/4;
		else
			i_end = 20*3*Sample_Size_Unit/4;
			
		i_flag = ((number - 1)*3 + sub_number)*Sample_Size_Unit/4;
		Address_S = ADDR_FLASH_SECTOR_5;
		Address_T = ADDR_FLASH_SECTOR_7;
		//copy data from sector5 to sector7 
		for(uint32_t i = 0; i < i_end; i++)
		{
			if(i >= i_flag && i < i_flag + Sample_Size_Unit/4)
			{
				temp_data = data[i - i_flag];
			}
			else
			{
				temp_data = *(__IO uint32_t*)Address_S;
			}
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_T, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
		//copy data from sector7 back to sector5
		Address_S = ADDR_FLASH_SECTOR_5;
		Address_T = ADDR_FLASH_SECTOR_7;
		EraseFlash(ADDR_FLASH_SECTOR_5, ADDR_FLASH_SECTOR_5);
		
		for(uint32_t i = 0; i < i_end; i++)
		{
			temp_data = *(__IO uint32_t*)Address_T;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_S, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}	
	}
	
	if(number > 20)
	{
		i_end = (SampleNumber - 20)*3*Sample_Size_Unit/4;
		i_flag = ((number - 20 - 1)*3 + sub_number)*Sample_Size_Unit/4;
		Address_S = ADDR_FLASH_SECTOR_6;
		Address_T = ADDR_FLASH_SECTOR_7;
		//copy data from sector6 to sector7 
		for(uint32_t i = 0; i < i_end; i++)
		{
			if(i >= i_flag && i < i_flag + Sample_Size_Unit/4)
			{
				temp_data = data[i - i_flag];
			}
			else
			{
				temp_data = *(__IO uint32_t*)Address_S;
			}
			
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_T, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}
		//copy data from sector7 back to sector6
		Address_S = ADDR_FLASH_SECTOR_6;
		Address_T = ADDR_FLASH_SECTOR_7;
		EraseFlash(ADDR_FLASH_SECTOR_6, ADDR_FLASH_SECTOR_6);
		for(uint32_t i = 0; i < i_end; i++)
		{
			temp_data = *(__IO uint32_t*)Address_T;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address_S, temp_data) == HAL_OK)
			{
				Address_T = Address_T + 4;
				Address_S = Address_S + 4;
			}
			else
			{ 
				sprintf((char*)Error_Message, "[ERROR] %s[%d]: %s\n", __FILE__, __LINE__, __func__);
				Error_Handler();
			}
		}	
	}
	EraseFlash(ADDR_FLASH_SECTOR_7, ADDR_FLASH_SECTOR_7);
	HAL_FLASH_Lock();	
	return 0;
}

uint8_t Send_One_Flash_Sample(uint8_t number)
{
	uint32_t Address = ADDR_FLASH_SECTOR_4;
	uint8_t SampleNumber = *(__IO uint8_t*)Address;
	if(number == 0 || number > SampleNumber)
		return 1;
	uint32_t data;
	if(number < 21)
	{
		Address = ADDR_FLASH_SECTOR_5 + (number -1)*3*Sample_Size_Unit;
		for(uint8_t j = 0; j < 3; j++)
		{
			for(uint32_t i = 0; i < Sample_Size_Unit; i++)
			{
				data = *(__IO uint8_t*)Address;
				printf("%d ", data);
				Address = Address +1;
			}
			printf("\n");
		}
	}
	if(number > 20)
	{
		Address = ADDR_FLASH_SECTOR_6 + (number - 20 - 1)*3*Sample_Size_Unit;
		for(uint8_t j = 0; j < 3; j++)
		{
			for(uint32_t i = 0; i < Sample_Size_Unit; i++)
			{
				data = *(__IO uint8_t*)Address;
				printf("%d ", data);
				Address = Address +1;
			}
			printf("\n");
		}
	}
	return 0;
}

void FVD_FLASH_EnableRDP(void)
{
	/*
	RDP_LEVEL_0: for test
	RDP_LEVEL_1: test by others
	RDP_LEVEL_2: for product
	*/
	FLASH_OBProgramInitTypeDef FLASH_OBProgramInitStruct;
	HAL_FLASHEx_OBGetConfig(&FLASH_OBProgramInitStruct);
	
	if(FLASH_OBProgramInitStruct.RDPLevel == OB_RDP_LEVEL_0)
	{
		HAL_FLASH_OB_Unlock();
		HAL_FLASH_Unlock();
		
		FLASH_OBProgramInitStruct.OptionType = OPTIONBYTE_RDP;
		FLASH_OBProgramInitStruct.RDPLevel = OB_RDP_LEVEL_1;
		HAL_FLASHEx_OBProgram(&FLASH_OBProgramInitStruct);
		
		if(HAL_FLASH_OB_Launch() != HAL_OK)
		{
			Error_Handler();
		}
		
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
	}

	return;
}

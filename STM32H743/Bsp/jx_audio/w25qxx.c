#include "w25qxx.h"
#include "delay.h"

static SPI_HandleTypeDef hspi4;
static DMA_HandleTypeDef hdma_spi4_rx;
u16 W25QXX_TYPE=W25Q256;	    

uint8_t DMA1State=0;    //1æµ
uint8_t SPI4RxState=0;  //1���

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q256
//����Ϊ32M�ֽ�,����512��Block,8192��Sector 
													 
//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{ 
	u8 temp;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();           
    
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
	W25QXX_CS(1);			               
	SPI4_Init();		   			        
	W25QXX_TYPE=W25QXX_ReadID();	        
	if(W25QXX_TYPE==W25Q256)                
	{
		temp=W25QXX_ReadSR(3);              
		if((temp&0X01)==0)			        
		{
			W25QXX_CS(0); 			       
			SPI4_ReadWriteByte(W25X_Enable4ByteAddr);   
			W25QXX_CS(1);       		        
		}
	}
	W25QXX_WAKEUP();
}  

//��ȡW25QXX��״̬�Ĵ�����W25QXXһ����3��״̬�Ĵ���
//״̬�Ĵ���1��
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
//״̬�Ĵ���2��
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//״̬�Ĵ���3��
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:״̬�Ĵ����ţ���:1~3
//����ֵ:״̬�Ĵ���ֵ
u8 W25QXX_ReadSR(u8 regno)   
{  
	u8 byte=0,command=0; 
    switch(regno)
    {
        case 1:
            command=W25X_ReadStatusReg1;    //��״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //��״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //��״̬�Ĵ���3ָ��
            break;
        default:
            command=W25X_ReadStatusReg1;    
            break;
    }    
	W25QXX_CS(0);                            //ʹ������   
	SPI4_ReadWriteByte(command);            //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI4_ReadWriteByte(0Xff);          //��ȡһ���ֽ�  
	W25QXX_CS(1);                            //ȡ��Ƭѡ     
	return byte;   
} 
//дW25QXX״̬�Ĵ���
void W25QXX_Write_SR(u8 regno,u8 sr)   
{   
    u8 command=0;
    switch(regno)
    {
        case 1:
            command=W25X_WriteStatusReg1;    //д״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //д״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //д״̬�Ĵ���3ָ��
            break;
        default:
            command=W25X_WriteStatusReg1;    
            break;
    }   
	W25QXX_CS(0);                            //ʹ������   
	SPI4_ReadWriteByte(command);            //����дȡ״̬�Ĵ�������    
	SPI4_ReadWriteByte(sr);                 //д��һ���ֽ�  
	W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
}   
//W25QXXдʹ��	
//��WEL��λ   
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS(0);                            //ʹ������   
    SPI4_ReadWriteByte(W25X_WriteEnable);   //����дʹ��  
	W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
} 
//W25QXXд��ֹ	
//��WEL����  
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS(0);                            //ʹ������   
    SPI4_ReadWriteByte(W25X_WriteDisable);  //����д��ָֹ��    
	W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
} 

//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
//0XEF18,��ʾоƬ�ͺ�ΪW25Q256
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;	  
	W25QXX_CS(0);				    
	SPI4_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPI4_ReadWriteByte(0x00); 	    
	SPI4_ReadWriteByte(0x00); 	    
	SPI4_ReadWriteByte(0x00); 	 			   
	Temp|=SPI4_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI4_ReadWriteByte(0xFF);	 
	W25QXX_CS(1);				    
	return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	W25QXX_CS(0);                            //ʹ������   
    SPI4_ReadWriteByte(W25X_ReadData);      //���Ͷ�ȡ����  
    if(W25QXX_TYPE==W25Q128)                //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI4_ReadWriteByte((u8)((ReadAddr)>>24));    
    }
    SPI4_ReadWriteByte((u8)((ReadAddr)>>16));   //����24bit��ַ    
    SPI4_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI4_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI4_ReadWriteByte(0XFF);    //ѭ������  
    }
	W25QXX_CS(1);  				    	      
}  

void W25QXX_ReadDMA(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
{
 	u16 i;
	uint8_t temp;
	
	W25QXX_CS(0);                            //ʹ������   
	SPI4_ReadWriteByte(W25X_ReadData);      //���Ͷ�ȡ����  
	if(W25QXX_TYPE==W25Q256)                //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
	{
			SPI4_ReadWriteByte((u8)((ReadAddr)>>24));    
	}
	SPI4_ReadWriteByte((u8)((ReadAddr)>>16));   //����24bit��ַ    
	SPI4_ReadWriteByte((u8)((ReadAddr)>>8));   
	SPI4_ReadWriteByte((u8)ReadAddr);   
//	for(i=0;i<NumByteToRead;i++)
//	{ 
//			pBuffer[i]=SPI4_ReadWriteByte(0XFF);    //ѭ������  
//	}
	temp = DMA1State;
	while(temp == 1)
	{
		temp = DMA1State;
	}
	DMA1State = 1;
	SPI4_DmaInit();
	Spi4_RxMode();
	SPI4RxState = 0;
	HAL_SPI_DMAStop(&hspi4);
	HAL_SPI_Receive_DMA (&hspi4, pBuffer, NumByteToRead);
	temp = SPI4RxState;
	while(temp == 0)
	{
		temp = SPI4RxState;
	}
	DMA1State = 0;
	Spi4_RxTxMode();
	W25QXX_CS(1);  				    	      
}
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    W25QXX_Write_Enable();                  //SET WEL 
	W25QXX_CS(0);                            //ʹ������   
    SPI4_ReadWriteByte(W25X_PageProgram);   //����дҳ����   
    if(W25QXX_TYPE==W25Q256)                //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI4_ReadWriteByte((u8)((WriteAddr)>>24)); 
    }
    SPI4_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
    SPI4_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI4_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI4_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	W25QXX_CS(1);                            //ȡ��Ƭѡ 
	W25QXX_Wait_Busy();					   //�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
u8 W25QXX_BUFFER[4096];		 
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * W25QXX_BUF;	  
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}
//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                  //SET WEL 
    W25QXX_Wait_Busy();   
  	W25QXX_CS(0);                            //ʹ������   
    SPI4_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
	W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
	W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ������������ʱ��:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
	//����falsh�������,������   
 	//printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
	W25QXX_Write_Enable();                  //SET WEL 	 
	W25QXX_Wait_Busy();   
	W25QXX_CS(0);                            //ʹ������   
	SPI4_ReadWriteByte(W25X_SectorErase);   //������������ָ�� 
	if(W25QXX_TYPE==W25Q256)                //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
	{
			SPI4_ReadWriteByte((u8)((Dst_Addr)>>24)); 
	}
	SPI4_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
	SPI4_ReadWriteByte((u8)((Dst_Addr)>>8));   
	SPI4_ReadWriteByte((u8)Dst_Addr);  
	W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
  W25QXX_Wait_Busy();   				    //�ȴ��������
}  
//�ȴ�����
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR(1)&0x01)==0x01);   // �ȴ�BUSYλ���
}  
//�������ģʽ
void W25QXX_PowerDown(void)   
{ 
  	W25QXX_CS(0);                            //ʹ������   
    SPI4_ReadWriteByte(W25X_PowerDown);     //���͵�������  
	W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
    delay_us(3);                            //�ȴ�TPD  
}   
//����
void W25QXX_WAKEUP(void)   
{  
  	W25QXX_CS(0);                                //ʹ������   
    SPI4_ReadWriteByte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB    
	W25QXX_CS(1);                                //ȡ��Ƭѡ     	      
    delay_us(3);                                //�ȴ�TRES1
}   

void SPI4_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_SPI4_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_2LINES;
  hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi4.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi4.Init.NSS = SPI_NSS_SOFT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 7;
  hspi4.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi4.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi4.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi4.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi4.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi4.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi4.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi4.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
	SPI4_DmaInit();
//  HAL_NVIC_SetPriority(SPI4_IRQn, 2, 2);
//  HAL_NVIC_EnableIRQ(SPI4_IRQn);
	
  SPI4_ReadWriteByte(0Xff);                           //��������
	
}

void Spi4_RxMode(void)
{
  hspi4.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }	
}

void Spi4_RxTxMode(void)
{
  hspi4.Init.Direction = SPI_DIRECTION_2LINES;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }	
	
}

u8 SPI4_ReadWriteByte(u8 TxData)
{
	u8 Rxdata;
	HAL_SPI_TransmitReceive(&hspi4, &TxData, &Rxdata, 1, 10000);

 	return Rxdata;          		   		
}

void SPI4_DmaInit(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
	
	__HAL_LINKDMA(&hspi4,hdmarx,hdma_spi4_rx);
	
	hdma_spi4_rx.Instance = DMA1_Stream7;
	hdma_spi4_rx.Init.Request = DMA_REQUEST_SPI4_RX;
	hdma_spi4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_spi4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_spi4_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_spi4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_spi4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_spi4_rx.Init.Mode = DMA_NORMAL;
	hdma_spi4_rx.Init.Priority = DMA_PRIORITY_HIGH;
	hdma_spi4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	if (HAL_DMA_Init(&hdma_spi4_rx) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 2, 2);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
	
}

void DMA1_Stream7_IRQHandler(void)
{
  //HAL_DMA_IRQHandler(&hdma_spi4_rx);
	if(((DMA1->HISR) & 0x08000000) == 0x08000000)
	{
		SPI4RxState = 1;
	}
	DMA1->HIFCR = 0x0F400000;
}

void SPI4_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi4);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
}

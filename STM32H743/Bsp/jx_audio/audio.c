#include "audio.h"
#include "string.h"
#include "w25qxx.h"
#include "usart.h"
#include "delay.h"

//音频原始数据
//#include "1.h"  
//#include "2.h"  
//#include "3.h"  
//#include "4.h"  
//#include "5.h"  
//#include "6.h"  
//#include "7.h"  
//#include "8.h"  
//#include "9.h"  
//#include "10.h" 
//#include "11.h" 
//#include "12.h" 
//#include "13.h" 
//#include "14.h" 
//#include "15.h" 
//#include "16.h" 
//#include "17.h" 
//#include "18.h" 
//#include "19.h" 
//#include "20.h" 
//#include "21.h" 
//#include "22.h" 
//#include "23.h" 

//音频DAC数据                //大小      	//地址       //音频内容
#include "audiodata1.h"    //33120			0            //请输入密码  
//#include "audiodata2.h"    //46880			33120        //密码错误，请重新输入
//#include "audiodata3.h"    //36000			80000        //密码验证通过
//#include "audiodata4.h"    //36800			116000       //请输入新密码
//#include "audiodata5.h"    //57440			152800       //密码不一致，请重新输入
//#include "audiodata6.h"    //37040			210240       //密码修改成功
//#include "audiodata7.h"    //56240			247280       //第一次采集，请放入手指
//#include "audiodata8.h"    //56800			303520       //第二次采集，请放入手指
//#include "audiodata9.h"    //57760			360320       //第三次采集，请放入手指
//#include "audiodata10.h"   //55680			418080       //采集完成，请移开手指
//#include "audiodata11.h"   //28320			473760       //注册成功
//#include "audiodata12.h"   //54880			502080       //注册失败，请重新注册
//#include "audiodata13.h"   //51680			556960       //是否删除所有静脉样本
//#include "audiodata14.h"   //96480			608640       //设置为常用模式请按“1”，设置为安全模式请按“2” 
//#include "audiodata15.h"   //68800			705120       //尚未注册指静脉，请注册指静脉  
//#include "audiodata16.h"   //55600  		773920       //正在连接网络，请稍后
//#include "audiodata17.h"   //36320			829520       //网络连接成功
//#include "audiodata18.h"   //78800			865840       //网络连接失败，请检查网络后再试一次!!!
//#include "audiodata19.h"   //33120			944640       //请放入手指
//#include "audiodata20.h"   //58800			977760       //指静脉验证通过，请开箱
//#include "audiodata21.h"   //64240			1036560      //指静脉验证失败，请再试一次
//#include "audiodata22.h"   //40720			1100800      //警戒模式已打开
//#include "audiodata23.h"   //56320  		1141520      //电量不足，请更换电池

static DAC_HandleTypeDef hdac1;
static DMA_HandleTypeDef hdma_dac1_ch1;
static TIM_HandleTypeDef htim6;

static MUSIC_INFO music_info;
static AUDIO_PLAY audio_play;
uint8_t databuf[4096];

const AUDIO_LIST audio_list =
{
	{33120, 46880, 36000,  36800,  57440,  37040,  56240,  56800,  57760,  55680,  
	 28320,  54880,  51680,  96480,  68800,  55600,  36320,  78800,  33120,  58800,   
	 64240,   40720,   56320},
	{    0, 33120, 80000, 116000, 152800, 210240, 247280, 303520, 360320, 418080, 
	 473760, 502080, 556960, 608640, 705120, 773920, 829520, 865840, 944640, 977760, 
	 1036560, 1100800, 1141520}   
};

void JX_WriteDataToFlash(uint8_t number)
{
	uint32_t i, j, value1, value2, subaddr;
	uint16_t *p;
	
	number = -1 + 1;  //1-23
	p = (uint16_t *)audiodata1;
	
	for(i=0, subaddr=0; i<number; i++)
	{
		value1 = audio_list.data_number[i]*2 / 4096;
		value2 = audio_list.data_number[i]*2 % 4096;
		if(value2>0)
			value1 += 1;
    subaddr += value1*4096;
	}
	value1 = audio_list.data_number[number]*2 / 4096;
	value2 = audio_list.data_number[number]*2 % 4096;
	if(value2>0)
		value1 += 1;

	//总字节数 = value1*4096 + value2
	//存储首地址 = subaddr
	for(j=0; j<value1; j++)
	{
		if(j == value1-1)
		{
			for(i=0; i<value2; i++)
			{
				if(i%2 == 0)
					databuf[i] = p[j*2048 + i/2];
				else
					databuf[i] = (p[j*2048 + i/2])>>8;
			}					
			for(i=value2; i<4096; i++)
			{
				if(i%2 == 0)
					databuf[i] = 0xff;
				else
					databuf[i] = 0x07;
			}					
		}
		else 
		{
			for(i=0; i<4096; i++)
			{
				if(i%2 == 0)
					databuf[i] = p[j*2048 + i/2];
				else
					databuf[i] = (p[j*2048 + i/2])>>8;
			}								
			
		}
		subaddr+=4096;
		W25QXX_Write(databuf, subaddr,4096);
	}
}

void JX_Audio_Init(void)
{	
	uint8_t i;
	
	JX_Audio_Dac1Init();
	JX_Audio_DMA_Init();
	W25QXX_Init();
	JX_Audio_Time6Init(124, 99);	
	HAL_TIM_Base_Start(&htim6);
}

void JX_Audio_DeInit(void)
{
	__HAL_RCC_DAC12_CLK_DISABLE();
	__HAL_RCC_TIM6_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
	HAL_DMA_DeInit(hdac1.DMA_Handle1);
}


void JX_Audio_Play(int32_t idx)
{
	uint32_t i;
  uint32_t value1, value2, subaddr;
	
	for(i=0, subaddr=0; i<idx; i++)
	{
		value1 = audio_list.data_number[i]*2 / 4096;
		value2 = audio_list.data_number[i]*2 % 4096;
		if(value2>0)
			value1 += 1;
			subaddr += value1*4096;
	}
	value1 = audio_list.data_number[idx]*2 / 4096;
	value2 = audio_list.data_number[idx]*2 % 4096;
	if(value2>0)
		value1 += 1;
	HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
	audio_play.data_addr = subaddr;  //文件地址
	audio_play.data_size = value1;   //扇区大小
	audio_play.temp_addr = 0;				 //当前扇区
	audio_play.buf_flag=0;					 //缓冲标志
	W25QXX_ReadDMA((uint8_t*)(audio_play.Buf1), audio_play.data_addr +  audio_play.temp_addr * 4096, 16384);
	audio_play.temp_addr += 4;
	W25QXX_ReadDMA((uint8_t*)(audio_play.Buf2), audio_play.data_addr +  audio_play.temp_addr * 4096 ,16384);
	audio_play.temp_addr += 4; 
	HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)(audio_play.Buf1), 8192, DAC_ALIGN_12B_R);	

}

void JX_Audio_StopPaly(void)
{
	HAL_TIM_Base_Stop(&htim6);
	HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
	HAL_DMA_Abort_IT(&hdma_dac1_ch1);
}

static uint32_t JX_Audio_FormatConversion(uint8_t *pointer, uint8_t size)
{
	uint32_t value;
	
	if(size ==2)
		value =  pointer[0] | pointer[1]<<8 ;
	else
		value =  pointer[0] | pointer[1]<<8 | pointer[2]<<16 | pointer[3]<<24;
	
	return value;
}

static int32_t JX_Audio_AnalyticalData(uint8_t *data_pointer)
{
	uint8_t compare_state;
		//判断是否为WAVE文件
	compare_state = strncmp ( (char*)data_pointer, "RIFF", 4 );
	if(compare_state ==0)
	{
		music_info.riff_info.size = JX_Audio_FormatConversion(data_pointer + 4, 4);
		music_info.riff_info.type = JX_Audio_FormatConversion(data_pointer + 8, 4);
		compare_state = strncmp ( (char*)data_pointer + 8, "WAVE", 4 );
		if(compare_state)
			return -2;
		data_pointer += 12;
	}
	else return -1;
	
	//获取音频信息
	compare_state = strncmp ( (char*)data_pointer, "fmt ", 4 );
	if(compare_state ==0)
	{
		music_info.format_info.size = JX_Audio_FormatConversion(data_pointer + 4, 4);
		music_info.format_info.format_tag = JX_Audio_FormatConversion(data_pointer + 8, 2);
		music_info.format_info.channels = JX_Audio_FormatConversion(data_pointer + 10, 2);
		music_info.format_info.samples_per_ser = JX_Audio_FormatConversion(data_pointer + 12, 4);
		music_info.format_info.avg_bytes_per_sec = JX_Audio_FormatConversion(data_pointer + 16, 4);
		music_info.format_info.block_align = JX_Audio_FormatConversion(data_pointer + 20, 2);
		music_info.format_info.bits_per_sample = JX_Audio_FormatConversion(data_pointer + 22, 2);
		if(music_info.format_info.size == 16)
			music_info.format_info.extra_info = 0;
		else
			music_info.format_info.extra_info = 1;
		data_pointer += music_info.format_info.size + 8;
	}
	else return -3;
	
	//获取音频数据
	compare_state = strncmp ( (char*)data_pointer, "data", 4 );
	if(compare_state == 0)
	{
		music_info.data_info.size = JX_Audio_FormatConversion(data_pointer + 4, 4);
		music_info.data_info.pointer = data_pointer + 8;
	}
	else return -4 ;
	
	return 0;
}

//DAC初始化
static void JX_Audio_Dac1Init(void)
{
  DAC_ChannelConfTypeDef sConfig;
	GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_DAC12_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
}

//定时器初始化
static void JX_Audio_Time6Init(uint16_t Prescaler, uint32_t Period)
{
  TIM_MasterConfigTypeDef sMasterConfig;
	
  __HAL_RCC_TIM6_CLK_ENABLE();

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = Prescaler;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = Period;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
//	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 3, 2);
//	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
//	HAL_TIM_Base_Start_IT(&htim6);

}

//DMA初始化
static void JX_Audio_DMA_Init(void)
{
  __HAL_RCC_DMA2_CLK_ENABLE();
	
  __HAL_LINKDMA(&hdac1,DMA_Handle1,hdma_dac1_ch1);

	hdma_dac1_ch1.Instance = DMA2_Stream7;
	hdma_dac1_ch1.Init.Request = DMA_REQUEST_DAC1;
	hdma_dac1_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdma_dac1_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_dac1_ch1.Init.MemInc = DMA_MINC_ENABLE;
	hdma_dac1_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma_dac1_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma_dac1_ch1.Init.Mode = DMA_NORMAL;
	hdma_dac1_ch1.Init.Priority = DMA_PRIORITY_HIGH;
	hdma_dac1_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	if (HAL_DMA_Init(&hdma_dac1_ch1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 3, 3);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

void JX_Audio_ConversionFileToHex(int32_t idx)
{
	uint8_t *music_data_pointer ; 
	uint32_t i;
	uint16_t databuffer;
	
	//获取文件指针
	switch(idx)
	{
//		case 1 :
//			music_data_pointer = (uint8_t *)sound1;
//			break;
//		case 2 :
//			music_data_pointer = (uint8_t *)sound2;
//			break;
//		case 3 :
//			music_data_pointer = (uint8_t *)sound3;
//			break;
//		case 4 :
//			music_data_pointer = (uint8_t *)sound4;
//			break;
//		case 5 :
//			music_data_pointer = (uint8_t *)sound5;
//			break;
//		case 6 :
//			music_data_pointer = (uint8_t *)sound6;
//			break;
//		case 7 :
//			music_data_pointer = (uint8_t *)sound7;
//			break;
//		case 8 :
//			music_data_pointer = (uint8_t *)sound8;
//			break;
//		case 9 :
//			music_data_pointer = (uint8_t *)sound9;
//			break;
//		case 10 :
//			music_data_pointer = (uint8_t *)sound10;
//			break;
//		case 11 :
//			music_data_pointer = (uint8_t *)sound11;
//			break;
//		case 12 :
//			music_data_pointer = (uint8_t *)sound12;
//			break;
//		case 13 :
//			music_data_pointer = (uint8_t *)sound13;
//			break;
//		case 14 :
//			music_data_pointer = (uint8_t *)sound14;
//			break;
//		case 15 :
//			music_data_pointer = (uint8_t *)sound15;
//			break;
//		case 16 :
//			music_data_pointer = (uint8_t *)sound16;
//			break;
//		case 17 :
//			music_data_pointer = (uint8_t *)sound17;
//			break;
//		case 18 :
//			music_data_pointer = (uint8_t *)sound18;
//			break;
//		case 19 :
//			music_data_pointer = (uint8_t *)sound19;
//			break;
//		case 20 :
//			music_data_pointer = (uint8_t *)sound20;
//			break;
//		case 21 :
//			music_data_pointer = (uint8_t *)sound21;
//			break;
//		case 22 :
//			music_data_pointer = (uint8_t *)sound22;
//			break;
//		case 23 :
//			music_data_pointer = (uint8_t *)sound23;
//			break;
		default :
			return ;
	}
	//获取文件信息
	if(JX_Audio_AnalyticalData(music_data_pointer))
		return ;	
	printf("#ifndef _audiodata _H\n\r");
	printf("#define _audiodata _H\n\r");
	printf("\n\r");
	printf("#include \"stm32h7xx_hal.h\"\n\r");
	printf("#include \"audio.h\"\n\r");
	printf("\n\r");
	printf("/*File Information:\n\r");
	printf("format_tag        : %#05x\n\r", music_info.format_info.format_tag);
	printf("channels          : %#05x\n\r", music_info.format_info.channels);
	printf("samples_per_ser   : %#05x\n\r", music_info.format_info.samples_per_ser);
	printf("avg_bytes_per_sec : %#05x\n\r", music_info.format_info.avg_bytes_per_sec);
	printf("block_align       : %#05x\n\r", music_info.format_info.block_align);
	printf("bits_per_sample   : %#05x\n\r", music_info.format_info.bits_per_sample);
	printf("\n\r");
	printf("*/\n\r");
	printf("\n\r");
	printf("const uint16_t audiodata[] = {\n\r");
	for(i = 0; i<music_info.data_info.size/2; i++)
	{
		if(i%15 == 0 )
			printf("\n\r");
		databuffer = *(music_info.data_info.pointer + i * 2) | *(music_info.data_info.pointer + i * 2 + 1)<<8;
		databuffer += 0x7FFF;
		databuffer >>= 4;
		printf(" %#05x, ", databuffer);
		delay_us(2);
	}
	printf("};\n\r");
	
	printf("\n\r");
	
	printf("const DAC_DATA_INFO dac_data_info=\n\r");
	printf("				{\n\r");
	printf("					16000,\n\r");
	printf("					%d,\n\r", i);
	printf("					(uint16_t *)audiodata \n\r");
	printf("				};\n\r");
	printf("\n\r");
	printf("#endif\n\r");
}

void DMA2_Stream7_IRQHandler(void)
{
//	HAL_DMA_IRQHandler(&hdma_dac1_ch1);
	if(audio_play.temp_addr >= audio_play.data_size)
		HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
	if(((DMA2->HISR) & 0x08000000) == 0x08000000)
	{
		if(audio_play.data_size >= audio_play.temp_addr + 4)
		{
			if(audio_play.buf_flag == 0)
			{
				audio_play.buf_flag = 1;
				HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
				HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)(audio_play.Buf2), 8192, DAC_ALIGN_12B_R);
				W25QXX_ReadDMA((uint8_t*)(audio_play.Buf1), audio_play.data_addr +  audio_play.temp_addr * 4096, 16384);
				audio_play.temp_addr += 4;
			}
			else
			{
				audio_play.buf_flag = 0;
				HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
				HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)(audio_play.Buf1), 8192, DAC_ALIGN_12B_R);	
				W25QXX_ReadDMA((uint8_t*)(audio_play.Buf2), audio_play.data_addr +  audio_play.temp_addr * 4096, 16384);
				audio_play.temp_addr += 4;
			}	
		}
		else if(audio_play.data_size >= audio_play.temp_addr)
		{
			if(audio_play.buf_flag == 0)
			{
				audio_play.buf_flag = 1;
				HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
				HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)(audio_play.Buf2), 8192, DAC_ALIGN_12B_R);
				W25QXX_ReadDMA((uint8_t*)(audio_play.Buf1), audio_play.data_addr +  audio_play.temp_addr * 4096, (audio_play.data_size-audio_play.temp_addr+1)*4096 );
				audio_play.temp_addr = audio_play.data_size;
			}
			else
			{
				audio_play.buf_flag = 0;
				HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
				HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)(audio_play.Buf1), 8192, DAC_ALIGN_12B_R);	
				W25QXX_ReadDMA((uint8_t*)(audio_play.Buf2), audio_play.data_addr +  audio_play.temp_addr * 4096, (audio_play.data_size-audio_play.temp_addr+1)*4096);
				audio_play.temp_addr = audio_play.data_size;
			}	
		}	
		else if(audio_play.data_size == audio_play.temp_addr)
		{
			if(audio_play.buf_flag == 0)
			{
				HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
				HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)(audio_play.Buf2), 2048*(audio_play.data_size%4+1), DAC_ALIGN_12B_R);
				audio_play.temp_addr+=1;
			}
			else
			{
				HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
				HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)(audio_play.Buf1), 2048*2048*(audio_play.data_size%4+1), DAC_ALIGN_12B_R);
				audio_play.temp_addr+=1;
			}
		}
		else
			HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
	}
	DMA2->HIFCR = 0x0F400000;
}


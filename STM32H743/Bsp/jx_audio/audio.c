#include "audio.h"
#include "string.h"
#include "usart.h"
#include "delay.h"

//音频原始数据
#include "sound1.h"  //你好
#include "sound2.h"  //箱门已打开
#include "sound3.h"  //hello
//音频DAC数据
#include "audiodata1.h"  //你好
#include "audiodata2.h"  //箱门已打开
#include "audiodata3.h"  //hello

static DAC_HandleTypeDef hdac1;
static DMA_HandleTypeDef hdma_dac1_ch1;
static TIM_HandleTypeDef htim6;

static MUSIC_INFO music_info;
	
void JX_Audio_Init(void)
{
	JX_Audio_Dac1Init();
	JX_Audio_DMA_Init();
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
	DAC_DATA_INFO *dac_data_info_p ; 
	
	switch(idx)
	{
		case 1 :
			dac_data_info_p = (DAC_DATA_INFO*)&dac_data_info1;
			break;
		case 2 :
			dac_data_info_p = (DAC_DATA_INFO*)&dac_data_info2;
			break;
		case 3 :
			dac_data_info_p = (DAC_DATA_INFO*)&dac_data_info3;
			break;

		default :
			return ;
	}
	if(dac_data_info_p->sampling_rate == 16000)
		JX_Audio_Time6Init(124, 99); 
	else {}
	
	HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)(dac_data_info_p->data_pointer), dac_data_info_p->data_length, DAC_ALIGN_12B_R);	
	HAL_TIM_Base_Start(&htim6);
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

  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 2, 2);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

void DMA2_Stream7_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_dac1_ch1);
}

void JX_Audio_ConversionFileToHex(int32_t idx)
{
	uint8_t *music_data_pointer ; 
	uint32_t i;
	uint16_t databuffer;
	
	//获取文件指针
	switch(idx)
	{
		case 1 :
			music_data_pointer = (uint8_t *)sound1;
			break;
		case 2 :
			music_data_pointer = (uint8_t *)sound2;
			break;
		case 3 :
			music_data_pointer = (uint8_t *)sound3;
			break;
		default :
			return ;
	}
	//获取文件信息
	if(JX_Audio_AnalyticalData(music_data_pointer))
		return ;	
	
	printf("Start!!\n\r");
	printf("\n\r");
	printf("File Information:\n\r");
	printf("format_tag        : %#05x\n\r", music_info.format_info.format_tag);
	printf("channels          : %#05x\n\r", music_info.format_info.channels);
	printf("samples_per_ser   : %#05x\n\r", music_info.format_info.samples_per_ser);
	printf("avg_bytes_per_sec : %#05x\n\r", music_info.format_info.avg_bytes_per_sec);
	printf("block_align       : %#05x\n\r", music_info.format_info.block_align);
	printf("bits_per_sample   : %#05x\n\r", music_info.format_info.bits_per_sample);
	printf("\n\r");
	printf("data!\n\r");
	for(i = 0; i<music_info.data_info.size/2; i++)
	{
		if(i%15 == 0 )
			printf("\n\r");
		databuffer = *(music_info.data_info.pointer + i * 2) | *(music_info.data_info.pointer + i * 2 + 1)<<8;
		databuffer += 0x7FFF;
		databuffer >>= 4;
		printf(" %#05x, ", databuffer);
		delay_us(10);
	}
	printf("\n\r");
	printf("the number of data : %d\n\r", i);
	printf("finished!!\n\r");
}


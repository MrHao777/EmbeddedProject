#ifndef _audio_H
#define _audio_H

#include "stm32h7xx_hal.h"

#define audio_number 23

typedef struct 
{
	uint32_t	size;
	uint32_t	type;
} RIFF_INFO;

typedef struct
{
	uint32_t size;
	uint16_t format_tag;
	uint16_t channels;
	uint32_t samples_per_ser;
	uint32_t avg_bytes_per_sec;
	uint16_t block_align;
	uint16_t bits_per_sample;
	uint16_t extra_info;
}	FORMAT_INFO;

typedef struct
{
	uint32_t size;
	uint32_t type;
} FACT_INFO;

typedef struct
{
	uint32_t size;
	uint8_t *pointer;
} DATA_INFO;

typedef struct
{
	RIFF_INFO riff_info;
	FORMAT_INFO format_info;
	FACT_INFO fact_info;
	DATA_INFO data_info;
} MUSIC_INFO;

typedef struct
{
	uint16_t sampling_rate;
	uint64_t data_length;
	uint16_t *data_pointer;
} DAC_DATA_INFO;

typedef struct
{
	uint32_t  data_number[audio_number];
	uint32_t  data_addr[audio_number];
} AUDIO_LIST;

typedef struct
{	
	uint32_t data_addr;        //文件地址
	uint32_t data_size;				 //扇区大小
	uint32_t temp_addr;        //当前扇区
	uint32_t Buf1[4096];       //缓冲区1
	uint32_t Buf2[4096];			 //缓冲区2
	uint8_t  buf_flag;         //缓冲区标志 0:buf1  1:buf2
} AUDIO_PLAY;

/*****************用户接口**********************/
void JX_Audio_Init(void);
void JX_Audio_DeInit(void);
void JX_Audio_Play(int32_t idx);
void JX_Audio_StopPaly(void);

/*****************内部接口**********************/
//将WAV文件转化为16进制数组并输出到串口
void JX_Audio_ConversionFileToHex(int32_t idx);
//将音频数据写入flash
void JX_WriteDataToFlash(uint8_t number);
//格式转换
static uint32_t JX_Audio_FormatConversion(uint8_t *pointer, uint8_t size);
//解析数据
static int32_t JX_Audio_AnalyticalData(uint8_t *data_pointer);
//DAC初始化
static void JX_Audio_Dac1Init(void);
//定时器初始化
static void JX_Audio_Time6Init(uint16_t Prescaler, uint32_t Period);
//DMA初始化
static void JX_Audio_DMA_Init(void);
//void DMA2_Stream7_IRQHandler(void);

#endif

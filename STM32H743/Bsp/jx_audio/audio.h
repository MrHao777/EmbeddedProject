#ifndef _audio_H
#define _audio_H

#include "stm32h7xx_hal.h"

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
	uint32_t data_length;
	uint16_t *data_pointer;
} DAC_DATA_INFO;

/*****************�û��ӿ�**********************/
void JX_Audio_Init(void);
void JX_Audio_DeInit(void);
void JX_Audio_Play(int32_t idx);
void JX_Audio_StopPaly(void);

/*****************�ڲ��ӿ�**********************/
//��WAV�ļ�ת��Ϊ16�������鲢���������
void JX_Audio_ConversionFileToHex(int32_t idx);
//��ʽת��
static uint32_t JX_Audio_FormatConversion(uint8_t *pointer, uint8_t size);
//��������
static int32_t JX_Audio_AnalyticalData(uint8_t *data_pointer);
//DAC��ʼ��
static void JX_Audio_Dac1Init(void);
//��ʱ����ʼ��
static void JX_Audio_Time6Init(uint16_t Prescaler, uint32_t Period);
//DMA��ʼ��
static void JX_Audio_DMA_Init(void);
//void DMA2_Stream7_IRQHandler(void);

#endif

#include "rtc.h"

static RTC_HandleTypeDef hrtc;

void JX_RTC_Init(void)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

	__HAL_RCC_RTC_ENABLE();
  __HAL_RCC_RTC_CLK_ENABLE();
	
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 0x7f;
  hrtc.Init.SynchPrediv = 0xff;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	HAL_PWR_EnableBkUpAccess();
	if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR0)!=0X5051)
	{ 
		JX_RTC_SetTime(2018, 11, 28, 16, 22, 0);
		HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0X5051);
	}
	HAL_PWR_DisableBkUpAccess();
}

void JX_RTC_DeInit(void)
{
	 __HAL_RCC_RTC_DISABLE();
}

void JX_RTC_GetTime(uint32_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *min, uint8_t *sec)
{
	RTC_DateTypeDef RTC_Date;
	RTC_TimeTypeDef RTC_Time;

	if(HAL_RTC_GetTime(&hrtc, &RTC_Time, RTC_FORMAT_BIN)!=HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	if(HAL_RTC_GetDate(&hrtc, &RTC_Date, RTC_FORMAT_BIN)!=HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	*year = RTC_Date.Year + 2000;
	*month = RTC_Date.Month;
	*day = RTC_Date.Date;
	*hour = RTC_Time.Hours;
	*min = RTC_Time.Minutes;
	*sec = RTC_Time.Seconds;
}

void JX_RTC_SetTime(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
	RTC_DateTypeDef RTC_DateStructure;
	RTC_TimeTypeDef RTC_TimeStructure;
	
	RTC_DateStructure.Date=day;
	RTC_DateStructure.Month=month;
	RTC_DateStructure.WeekDay=RTC_WEEKDAY_MONDAY;
	RTC_DateStructure.Year=year-2000;
	
	RTC_TimeStructure.Hours=hour;
	RTC_TimeStructure.Minutes=min;
	RTC_TimeStructure.Seconds=sec;
	RTC_TimeStructure.TimeFormat=RTC_HOURFORMAT12_AM;
	RTC_TimeStructure.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
  RTC_TimeStructure.StoreOperation=RTC_STOREOPERATION_RESET;
  HAL_PWR_EnableBkUpAccess();
	if(HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	if(HAL_RTC_SetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	HAL_PWR_DisableBkUpAccess();
}

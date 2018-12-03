#ifndef _rtc_H
#define _rtc_H

#include "stm32h7xx_hal.h"

void JX_RTC_Init(void);
void JX_RTC_DeInit(void);
void JX_RTC_GetTime(uint32_t *year, uint8_t *month, uint8_t *day, uint8_t *week, uint8_t *hour, uint8_t *min, uint8_t *sec);

/**  时间设置函数
  *  year(2000-2099)
	*  hour(0-23)
  *  week (1-7)
	*/
void JX_RTC_SetTime(uint32_t year, uint8_t month, uint8_t day, uint8_t week, uint8_t hour, uint8_t min, uint8_t sec);

#endif

#include "main.h"
#include "stm32h7xx_hal.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "LED.h"
#include "uh0002.h"
#include "rtc.h"
#include "dxi2c_stm32.h"
#include "audio.h"

int main(void)
{	
  volatile uint32_t dx8_authen_state;
	
  HAL_Init();
  SystemClock_Config();
	delay_init(400);
	uart_init(115200);
	
	JX_LED_Init();
	JX_RTC_Init();
	JX_UH0002_Init();
	JX_DX8_Init();
	JX_Audio_Init();
	dx8_authen_state = JX_DX8_Authen();
	
	JX_Audio_Play(2);	
//	JX_Audio_ConversionFileToHex(3);
  while (1)
  {
	 	JX_LED_Toggle;
		delay_ms(3000);
  }
}

void _Error_Handler(char *file, int line)
{
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

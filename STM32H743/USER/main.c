#include "main.h"
#include "stm32h7xx_hal.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "LED.h"
//#include "uh0002.h"
//#include "rtc.h"
//#include "dxi2c_stm32.h"
#include "audio.h"
//#include "flash.h"
//#include "LcdSpi.h"


/*
clk E2
cs  E4 SPI4
miso E5
mosi E6
*/

int main(void)
{	
	volatile uint32_t i;
	
  HAL_Init();
  SystemClock_Config();
	delay_init(400);
	uart_init(115200);
	
	JX_LED_Init();
//	JX_RTC_Init();
//	JX_Flash_Init();	
//	JX_LcdSpi_Init();
//	JX_UH0002_Init();
//	JX_DX8_Init();
	JX_Audio_Init();
	for(i=0; i<23; i++)
	{
		JX_Audio_Play(i);
		delay_ms(5000);
	}
	JX_LED_ON;	
  while (1)
	{
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

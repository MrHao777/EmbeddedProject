#include "main.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "key.h"
#include "ShockSensor.h"
#include "flash.h"
#include "microswitch.h"
#include "electromagnet.h"
#include "timer.h"
//#include "CommandStateMachine.h"

/**********引脚定义*********
ADC         : 		B1  ok
数据请求引脚:			D0  2
唤醒键      :         无
复位键      : 		A1  ok
设置键      :			A2  ok
微动开关    : 		A3  ok
震动传感器  :			A4  relay
电磁铁      :			    无

***************************/

int main(void)
{
  HAL_Init();
	Stm32_Clock_Init();
	delay_init(64);
	uart_init(115200);
	
	JX_ADC_Init();
	JX_KeyInit();
	JX_Shock_Init();
	JX_DoorState_Init();
	JX_TimerInit();
	JX_OpenDoor_Init();
//	JX_CommandStateMachine();
	while(1);
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
#endif 


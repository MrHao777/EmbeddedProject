#include "main.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "key.h"
#include "ShockSensor.h"
#include "power.h"
#include "flash.h"
#include "microswitch.h"
#include "electromagnet.h"
#include "timer.h"
#include "CommandStateMachine.h"

/*****定时器使用情况******************
time1  : 串口通信超时检测
time2  : 按键中断轮询 20ms
time3  : 开门状态计时
time4  : F1开机计时
*************************************/

/*****中断优先级列表******************
中断优先级分组 :NVIC_PRIORITYGROUP_2
--------------------------------------
中断名称            preemption     sub
MemoryManagement_IRQn        0       0
BusFault_IRQn                0       0
UsageFault_IRQn              0       0
SVCall_IRQn                  0       0
DebugMonitor_IRQn            0       0
PendSV_IRQn                  0       0
SysTick_IRQn                 0       0
USART1_IRQn                  1       1
EXTI9_5_IRQn                 2       2
TIM2_IRQn                    3       3
EXTI3_IRQn                   3       3
TIM3_IRQn                    3       3
EXTI4_IRQn                   3       3
TIM1_UP_IRQn                 3       2
TIM4_IRQn                    3       3
*************************************/

int main(void)
{
  HAL_Init();
	Stm32_Clock_Init();
	delay_init(64);
	uart_init(115200);
	
	delay_ms(100);//上电延时,等待唤醒脚状态稳定
	JX_KeyInit();
  JX_Shock_Init();
	JX_ADC_Init();
	JX_DoorState_Init();
	JX_TimerInit();
	JX_OpenDoor_Init();
	JX_FlashInit();
	JX_PowerCtrlInit();
	
	JX_CommandStateMachine();
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


#include "ShockSensor.h"
#include "delay.h"
#include "key.h"

uint8_t shock_sensor_state = 0;

void JX_Shock_Init(void)
{
	uint8_t i;
	for(i=0; i<50; i++)
	{
		delay_ms(100);
		if(key_Pressed)
		{
			JX_Shock_StopSensing();
			return ;
		}
	}
	JX_Shock_StartSensing();
}

void JX_Shock_DeInit(void)
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
	HAL_NVIC_DisableIRQ(EXTI3_IRQn);
}

void JX_Shock_StartSensing(void)
{
	shock_sensor_state = 1;
}

void JX_Shock_StopSensing(void)
{
	shock_sensor_state = 0;
}


#include "CommandStateMachine.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "key.h"
#include "ShockSensor.h"
#include "flash.h"
#include "microswitch.h"
#include "electromagnet.h"
#include "timer.h"
#include "string.h"

/***********状态标记**************
uint8_t     UartRequestState=0;              //从机数据请求标记
uint8_t     key_reset_state = 0;             //复位键标记             
uint8_t     key_set_state = 0;               //设置键标记
uint8_t     key_wakeup_state = 0;            //唤醒键标记
uint8_t     shock_sensor_state;              //震动传感器标记
uint8_t     timer_alarm_state = 0;           //定时器标记
uint16_t    USART_RX_STA=0;                  //接收状态标记
uint8_t     USART_RX_BUF[15];                //接收缓冲
uint8_t     UartNeedToRX=7;                  //接收期望字节数
**********************************/

#define USART_TX_LEN 7                       //串口发送缓冲区大小
static uint8_t USART_TX_BUF[USART_TX_LEN];   //数据发送缓冲
static uint8_t state = state_Nop;            //状态标志
void JX_CommandStateMachine(void)
{
	state = state_Nop;
	while(1)
	{
		switch(state)
		{
			case state_Nop :           /*空指令 处理各种标记*/
					if(UartRequestState)   //从机数据请求
					{
						delay_ms(10);
						UartRequestState = 0;
						state = state_ReceivePermit;
						break;
					}
					if(key_reset_state)    //复位键请求
					{
						delay_ms(10);//延时消抖,防止指令重复发送
						key_reset_state = 0;
						state = state_RequestRst;
						break;
					}
					if(key_set_state)      //设置键请求
					{
						delay_ms(10);
						key_set_state = 0;
						state = state_SetMode;
						break;
					}
					if(key_wakeup_state)   //唤醒键请求
					{
						delay_ms(10);
						key_wakeup_state = 0;
						state = state_WeekUpH7;
						break;
					}
					if(shock_sensor_state) //震动传感器
					{
						delay_ms(10);
						shock_sensor_state = 0;
						state = state_SendAlarmSig;
						break;
					}
				break;
			case state_ReceivePermit : /*数据接收请求允许*/
				JX_ClearTxBuf();
			  USART_TX_BUF[1] = state_ReceivePermit;
			  USART_TX_BUF[6] = JX_Usart_CRC8(USART_TX_BUF, 6);
			  USART_RX_STA = 0;
			  JX_TimerSetAlarmTime(2);
			  HAL_UART_Transmit(&huart1, USART_TX_BUF, 7, 2000);
			  while((USART_RX_STA&0x8000) == 0)
				{
					if(timer_alarm_state)
					{
						timer_alarm_state = 0;
						return; //超时返回
					}
				}
				if( USART_RX_BUF[0]==0x13 && USART_RX_BUF[1]==state_CrcResponse && USART_RX_BUF[6]  == JX_Usart_CRC8(USART_RX_BUF, 6))
				{
					if(USART_RX_BUF[3]==0)  
					{
						USART_RX_STA = 0;
						JX_TimerSetAlarmTime(2);
						while((USART_RX_STA&0x8000) == 0)
						{
							if(timer_alarm_state)
							{
								timer_alarm_state = 0;
								return;
							}
						}
						if(USART_RX_BUF[0]==0x13 && USART_RX_BUF[6]==JX_Usart_CRC8(USART_RX_BUF, 6))
							state = USART_RX_BUF[1];
					}
					else
					{
						state = state_ReceivePermit; //校验失败重新发送
					}
				}
				else
				{
					
				}
			  break;
			case state_RequestRst :    /*复位请求*/
				break;
			case state_SetMode :       /*设置请求*/
				break;
			case state_WeekUpH7:       /*唤醒请求*/
				break;
			case state_SendAlarmSig :  /*报警请求*/
				break;
      case state_CrcResponse :   /*crc应答*/
				break;
      case state_FinishRst :     /*复位完成*/
				break;
      case state_SendOpenTime :  /*发送开门时间*/
				break;
      case state_JudgeResult :   /*结果判断*/
				break;
      case state_JudgeUUID :     /*请求判断uuid*/
				break;
      case state_JudgeUserPwd :  /*请求判断用户密码*/
				break;
      case state_JudgeAdmiPwd :  /*请求判断管理员密码*/
				break;
      case state_SetAdmiPwd :    /*设置管理员密码*/
				break;
      case state_SetUserPwd :    /*设置用户密码*/
				break;
      case state_SetFingerprint :/*注册指纹*/
				break;
      case state_DelFingerprint :/*删除指纹*/
				break;
      case state_SetVerifMode :  /*设置验证模式(0:正常模式 1:安全模式)*/
				break;
      case state_SetWarnMode :   /*设置警戒模式(0:未开启 1:开启)*/
				break;
      case state_JPDO :          /*安全模式下验证密码,不开箱*/
				break;
      case state_JPJUO :         /*安全模式下验证密码和静脉,开箱*/
				break;
      case state_H7RstFinish :   /*H7复位完成*/
				break;
      case state_H7RequestSleep :/*H7请求休眠*/
			  break;
			default : 
				break;
		}
	}
}

static uint8_t JX_Usart_CRC8(uint8_t *message, uint8_t len)
{
    uint8_t crc;
    uint8_t i;
    crc = 0;
    while(len--)
    {
        crc ^= *message++;
        for(i = 0;i < 8;i++)
        {
            if(crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8c;
            }
                else crc >>= 1;
        }
    }
    return crc;
}

static void JX_ClearTxBuf(void)
{
	uint8_t i;
	for(i=1; i<USART_TX_LEN; i++)
		USART_TX_BUF[i] = 0;
	USART_TX_BUF[0] = 0x13;
}



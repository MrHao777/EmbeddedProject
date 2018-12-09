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

/***********״̬���**************
uint8_t     UartRequestState=0;              //�ӻ�����������
uint8_t     key_reset_state = 0;             //��λ�����             
uint8_t     key_set_state = 0;               //���ü����
uint8_t     key_wakeup_state = 0;            //���Ѽ����
uint8_t     shock_sensor_state;              //�𶯴��������
uint8_t     timer_alarm_state = 0;           //��ʱ�����
uint16_t    USART_RX_STA=0;                  //����״̬���
uint8_t     USART_RX_BUF[15];                //���ջ���
uint8_t     UartNeedToRX=7;                  //���������ֽ���
**********************************/

#define USART_TX_LEN 7                       //���ڷ��ͻ�������С
static uint8_t USART_TX_BUF[USART_TX_LEN];   //���ݷ��ͻ���
static uint8_t state = state_Nop;            //״̬��־
void JX_CommandStateMachine(void)
{
	state = state_Nop;
	while(1)
	{
		switch(state)
		{
			case state_Nop :           /*��ָ�� ������ֱ��*/
					if(UartRequestState)   //�ӻ���������
					{
						delay_ms(10);
						UartRequestState = 0;
						state = state_ReceivePermit;
						break;
					}
					if(key_reset_state)    //��λ������
					{
						delay_ms(10);//��ʱ����,��ָֹ���ظ�����
						key_reset_state = 0;
						state = state_RequestRst;
						break;
					}
					if(key_set_state)      //���ü�����
					{
						delay_ms(10);
						key_set_state = 0;
						state = state_SetMode;
						break;
					}
					if(key_wakeup_state)   //���Ѽ�����
					{
						delay_ms(10);
						key_wakeup_state = 0;
						state = state_WeekUpH7;
						break;
					}
					if(shock_sensor_state) //�𶯴�����
					{
						delay_ms(10);
						shock_sensor_state = 0;
						state = state_SendAlarmSig;
						break;
					}
				break;
			case state_ReceivePermit : /*���ݽ�����������*/
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
						return; //��ʱ����
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
						state = state_ReceivePermit; //У��ʧ�����·���
					}
				}
				else
				{
					
				}
			  break;
			case state_RequestRst :    /*��λ����*/
				break;
			case state_SetMode :       /*��������*/
				break;
			case state_WeekUpH7:       /*��������*/
				break;
			case state_SendAlarmSig :  /*��������*/
				break;
      case state_CrcResponse :   /*crcӦ��*/
				break;
      case state_FinishRst :     /*��λ���*/
				break;
      case state_SendOpenTime :  /*���Ϳ���ʱ��*/
				break;
      case state_JudgeResult :   /*����ж�*/
				break;
      case state_JudgeUUID :     /*�����ж�uuid*/
				break;
      case state_JudgeUserPwd :  /*�����ж��û�����*/
				break;
      case state_JudgeAdmiPwd :  /*�����жϹ���Ա����*/
				break;
      case state_SetAdmiPwd :    /*���ù���Ա����*/
				break;
      case state_SetUserPwd :    /*�����û�����*/
				break;
      case state_SetFingerprint :/*ע��ָ��*/
				break;
      case state_DelFingerprint :/*ɾ��ָ��*/
				break;
      case state_SetVerifMode :  /*������֤ģʽ(0:����ģʽ 1:��ȫģʽ)*/
				break;
      case state_SetWarnMode :   /*���þ���ģʽ(0:δ���� 1:����)*/
				break;
      case state_JPDO :          /*��ȫģʽ����֤����,������*/
				break;
      case state_JPJUO :         /*��ȫģʽ����֤����;���,����*/
				break;
      case state_H7RstFinish :   /*H7��λ���*/
				break;
      case state_H7RequestSleep :/*H7��������*/
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



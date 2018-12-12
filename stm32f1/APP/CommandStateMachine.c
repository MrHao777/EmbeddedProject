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

#define USART_TX_LEN 8                       //���ڷ��ͻ�������С
static uint8_t USART_TX_BUF[USART_TX_LEN];   //���ݷ��ͻ���
static int8_t RecvState = 0;                 //���ݽ���״̬
static uint8_t temp1, temp2, i;              //��ʱ����
static uint8_t temp_databuf[15];             //���ݻ���
static uint8_t temppassword[6];              //��ʱ����
static uint32_t tempuuid;                    //��ʱuuid

void JX_CommandStateMachine(void)
{
	static uint8_t state = state_Nop;  //״̬��־
	
	while(1)
	{
		switch(state)
		{
			case state_Nop :           /*��ָ�� ������ֱ��*/
				if(UartRequestState)   //�ӻ���������
				{
					delay_ms(50);
					UartRequestState = 0;
					state = state_ReceivePermit;
					break;
				}
				if(key_reset_state)    //��λ������
				{
					delay_ms(100);
					key_reset_state = 0;
					state = state_ReceivePermit;//state_RequestRst
					break;
				}
				if(key_set_state)      //���ü�����
				{
					delay_ms(100);
					key_set_state = 0;
					state = state_WeekUpH7;//   state_WeekUpH7
					break;
				}
				if(key_wakeup_state)   //���Ѽ�����
				{
					delay_ms(100);
					key_wakeup_state = 0;
					state = state_WeekUpH7;
					break;
				}
				if(shock_sensor_state) //�𶯴�����
				{
					delay_ms(100);
					shock_sensor_state = 0;
					state = state_SendAlarmSig;
					break;
				}
				break;
			case state_ReceivePermit : /*���ݽ�����������*/
				JX_UartSendCmdFrame(state_ReceivePermit, 0, 0, 0, 0);
			  RecvState = JX_UartRecvCmdFrame();
			  if(RecvState == 0)      //���ճɹ�
				{
					if(USART_RX_BUF[1]>='a' && USART_RX_BUF[1]<='m')
					{
						for(i=0; i<7; i++)
							temp_databuf[i] = USART_RX_BUF[i];//��������
						if(temp_databuf[4] != 0)
							UartNeedToRX = temp_databuf[4] + 2;
						JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);//����crc��Ӧ
						state = temp_databuf[1];
					}
					else
						state = state_Nop;
				}
        else if(RecvState == -1)//��ʱ
					state = state_Nop;
				else                    //crcʧ��
					state = state_ReceivePermit + 1;
				UartRequestState = 0;
			  break;
			case state_ReceivePermit + 1 :/*���ݽ�����������+1*/
				JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
			  RecvState = JX_UartRecvCmdFrame();
			  if(RecvState == 0)
				{
					if(USART_RX_BUF[1]>='a' && USART_RX_BUF[1]<='m')
					{
						for(i=0; i<7; i++)
							temp_databuf[i] = USART_RX_BUF[i];//��������
						if(temp_databuf[4] != 0)
							UartNeedToRX = temp_databuf[4] + 2;
						JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);//����crc��Ӧ
						state = temp_databuf[1];
					}
					else
						state = state_Nop;
				}
        else if(RecvState == -1)
					state = state_Nop;
				else 
					state = state_ReceivePermit + 1;
				UartRequestState = 0;
				break;
			case state_RequestRst :    /*��λ����*/
				JX_UartSendCmdFrame(state_RequestRst, 0, 0, 0, 0);
			  RecvState = JX_UartRecvCmdFrame();
			  if(RecvState == -1)
					state = state_Nop;
				else
				{
					USART_RX_STA = 0;
					if(USART_RX_BUF[3] == 0)  
						state = state_H7RstFinish;
					else
						state = state_RequestRst;
				}
				key_reset_state = 0;
				break;
			case state_SetMode :       /*��������*/
				JX_UartSendCmdFrame(state_SetMode, 0, 0, 0, 0);
			  RecvState = JX_UartRecvCmdFrame();
			  if(RecvState == -1)
					state = state_Nop;
				else
				{
					if(USART_RX_BUF[3] == 0)  
						state = state_Nop;
					else
						state = state_SetMode;
				}
				key_set_state = 0;
				break;
			case state_WeekUpH7:       /*��������*/
				temp1 = 0;
				if(JX_GetAlertMode())
					temp1 |= 0x02;
				else
					temp1 &= 0xfd;
				if(JX_GetOpenDoorMode())
			    temp1 |= 0x01;
				else
					temp1 &= 0xfe;
//				temp2 = JX_ADC_GetPower();
				temp2 = 20;
				JX_UartSendCmdFrame(state_WeekUpH7, temp1, temp2, 0, 0);
			  RecvState = JX_UartRecvCmdFrame();
			  if(RecvState == -1)
					state = state_Nop;
				else
				{
					if(USART_RX_BUF[3] == 0)  
						state = state_Nop;
					else
						state = state_WeekUpH7;
				}
				key_set_state = 0; //
				break;
			case state_SendAlarmSig :  /*��������*/
				if(JX_GetAlertMode())
				{
					temp1 = 0;
					if(JX_GetAlertMode())
						temp1 |= 0x02;
					else
						temp1 &= 0xfd;
					if(JX_GetOpenDoorMode())
						temp1 |= 0x01;
					else
						temp1 &= 0xfe;
					temp2 = JX_ADC_GetPower();
					JX_UartSendCmdFrame(state_WeekUpH7, temp1, temp2, 0, 0);
					RecvState = JX_UartRecvCmdFrame();
					if(RecvState == -1)
						state = state_Nop;
					else
					{
						if(USART_RX_BUF[3] != 0)
						{
							state = state_SendAlarmSig;
              break;
						}							
					}
					JX_UartSendCmdFrame(state_SendAlarmSig, 15, 0, 0, 0);
					RecvState = JX_UartRecvCmdFrame();
					if(RecvState == -1)
						state = state_Nop;
					else
					{
						if(USART_RX_BUF[3] == 0)  
							state = state_Nop;
						else
							state = state_SendAlarmSig;
					}
				}
				shock_sensor_state = 0;
				break;
      case state_FinishRst :     /*��λ���*/
				JX_DeletUUID();
			  JX_UartSendCmdFrame(state_FinishRst, 0, 0, 0, 0);
			  RecvState = JX_UartRecvCmdFrame();
			  if(RecvState == 0)
				{
					state = state_Nop;
				}else if(RecvState == -1)
				{
					state = state_Nop;
				}
				else
				{
					state = state_FinishRst;
				}
				break;
      case state_SendOpenTime :  /*���Ϳ���ʱ��*/
				break;
      case state_JudgeUUID :     /*�����ж�uuid*/
			  RecvState = JX_UartRecvDataFrame();
			  temp1 = temp_databuf[4];
			  if(RecvState == 0)
				{
					for(i=0; i<temp1; i++)
						temppassword[i] = USART_RX_BUF[i+1];
					for(; i<6; i++)
						temppassword[i] = 0;
					tempuuid = 0;                     //
					tempuuid |= temppassword[0]<<24;  //
					tempuuid |= temppassword[1]<<16;  //
					tempuuid |= temppassword[2]<<8;   //
					tempuuid |= temppassword[3];      //
					temp2 = JX_CheckUUID(tempuuid);   //
					if(temp2)
						temp2 = 1;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
				} else if(RecvState == -1)
				{
					state = state_Nop;
					break;
				} else
				{
					UartNeedToRX = temp_databuf[4] + 2;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
					state = state_JudgeUUID;
          break;					
				}
				delay_ms(10);
				JX_UartSendCmdFrame(state_JudgeResult, state_JudgeUUID, temp2, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_JudgeUUID, temp2, 0, 0);
				state = state_Nop;
				break;
      case state_JudgeUserPwd :  /*�����ж��û�����*/
			  RecvState = JX_UartRecvDataFrame();
			  temp1 = temp_databuf[4];
			  if(RecvState == 0)
				{
					for(i=0; i<temp1; i++)
						temppassword[i] = USART_RX_BUF[i+1];
					for(; i<6; i++)
						temppassword[i] = 0;
					temp2 = JX_CheckUserPassword(temppassword);
					if(temp2)
						temp2 = 1;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
				} else if(RecvState == -1)
				{
					state = state_Nop;
					break;
				} else
				{
					UartNeedToRX = temp_databuf[4] + 2;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
					state = state_JudgeUserPwd;
          break;					
				}
				delay_ms(10);
				JX_UartSendCmdFrame(state_JudgeResult, state_JudgeUserPwd, temp2, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_JudgeUserPwd, temp2, 0, 0);
				state = state_Nop;
				break;
      case state_JudgeAdmiPwd :  /*�����жϹ���Ա����*/
			  RecvState = JX_UartRecvDataFrame();
			  temp1 = temp_databuf[4];
			  if(RecvState == 0)
				{
					for(i=0; i<temp1; i++)
						temppassword[i] = USART_RX_BUF[i+1];
					for(; i<6; i++)
						temppassword[i] = 0;
					temp2 = JX_CheckAdmPassword(temppassword);
					if(temp2)
						temp2 = 1;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
				} else if(RecvState == -1)
				{
					state = state_Nop;
					break;
				} else
				{
					UartNeedToRX = temp_databuf[4] + 2;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
					state = state_JudgeAdmiPwd;
          break;					
				}
				delay_ms(10);
				JX_UartSendCmdFrame(state_JudgeResult, state_JudgeAdmiPwd, temp2, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_JudgeAdmiPwd, temp2, 0, 0);
				state = state_Nop;
				break;
      case state_SetAdmiPwd :    /*���ù���Ա����*/
			  RecvState = JX_UartRecvDataFrame();
			  temp1 = temp_databuf[4];
			  if(RecvState == 0)
				{
					for(i=0; i<temp1; i++)
						temppassword[i] = USART_RX_BUF[i+1];
					for(; i<6; i++)
						temppassword[i] = 0;
					JX_SetAdmPassword(temppassword); 
					JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
				} else if(RecvState == -1)
				{
					state = state_Nop;
					break;
				} else
				{
					UartNeedToRX = temp_databuf[4] + 2;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
					state = state_SetAdmiPwd;
          break;					
				}
				delay_ms(10);
				JX_UartSendCmdFrame(state_JudgeResult, state_SetAdmiPwd, 0, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_SetAdmiPwd, 0, 0, 0);
				state = state_Nop;
				break;
      case state_SetUserPwd :    /*�����û�����*/
			  RecvState = JX_UartRecvDataFrame();
			  temp1 = temp_databuf[4];
			  if(RecvState == 0)
				{
					for(i=0; i<temp1; i++)
						temppassword[i] = USART_RX_BUF[i+1];
					for(; i<6; i++)
						temppassword[i] = 0;
					JX_SetUserPassword(temppassword); 
					JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
				} else if(RecvState == -1)
				{
					state = state_Nop;
					break;
				} else
				{
					UartNeedToRX = temp_databuf[4] + 2;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
					state = state_SetUserPwd;
          break;					
				}
				delay_ms(10);
				JX_UartSendCmdFrame(state_JudgeResult, state_SetUserPwd, 0, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_SetUserPwd, 0, 0, 0);
				state = state_Nop;
				break;
      case state_SetFingerprint :/*ע��ָ��*/
			  RecvState = JX_UartRecvDataFrame();
			  temp1 = temp_databuf[4];
			  if(RecvState == 0)
				{
					for(i=0; i<temp1; i++)
						temppassword[i] = USART_RX_BUF[i+1];
					for(; i<6; i++)
						temppassword[i] = 0;
					tempuuid = 0;                     //
					tempuuid |= temppassword[0]<<24;  //
					tempuuid |= temppassword[1]<<16;  //
					tempuuid |= temppassword[2]<<8;   //
					tempuuid |= temppassword[3];      //
					JX_SaveUUID(tempuuid);            //
					JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
				} else if(RecvState == -1)
				{
					state = state_Nop;
					break;
				} else
				{
					UartNeedToRX = temp_databuf[4] + 2;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
					state = state_SetFingerprint;
          break;					
				}
				delay_ms(10);
				JX_UartSendCmdFrame(state_JudgeResult, state_SetFingerprint, 0, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_SetFingerprint, 0, 0, 0);
				state = state_Nop;
				break;
      case state_DelFingerprint :/*ɾ��ָ��*/
				JX_DeletUUID();
				JX_UartSendCmdFrame(state_JudgeResult, state_DelFingerprint, 0, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_DelFingerprint, 0, 0, 0);
				state = state_Nop;
				break;
      case state_SetVerifMode :  /*������֤ģʽ(0:����ģʽ 1:��ȫģʽ)*/
			  RecvState = JX_UartRecvDataFrame();
			  temp1 = temp_databuf[4];
			  if(RecvState == 0)
				{
					for(i=0; i<temp1; i++)
						temppassword[i] = USART_RX_BUF[i+1];
					for(; i<6; i++)
						temppassword[i] = 0;
					JX_SetOpenDoorMode(*(uint32_t*)temppassword);
					JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
				} else if(RecvState == -1)
				{
					state = state_Nop;
					break;
				} else
				{
					UartNeedToRX = temp_databuf[4] + 2;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
					state = state_SetVerifMode;
          break;					
				}
				delay_ms(10);
				JX_UartSendCmdFrame(state_JudgeResult, state_SetVerifMode, 0, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_SetVerifMode, 0, 0, 0);
				state = state_Nop;
				break;
      case state_SetWarnMode :   /*���þ���ģʽ(0:δ���� 1:����)*/
			  RecvState = JX_UartRecvDataFrame();
			  temp1 = temp_databuf[4];
			  if(RecvState == 0)
				{
					for(i=0; i<temp1; i++)
						temppassword[i] = USART_RX_BUF[i+1];
					for(; i<6; i++)
						temppassword[i] = 0;
					JX_SetAlertMode(*(uint32_t*)temppassword);
					JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
				} else if(RecvState == -1)
				{
					state = state_Nop;
					break;
				} else
				{
					UartNeedToRX = temp_databuf[4] + 2;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
					state = state_SetWarnMode;
          break;					
				}
				delay_ms(10);
				JX_UartSendCmdFrame(state_JudgeResult, state_SetWarnMode, 0, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_SetWarnMode, 0, 0, 0);
				state = state_Nop;
				break;
      case state_JPDO :          /*��ȫģʽ����֤����,������*/
			  RecvState = JX_UartRecvDataFrame();
			  temp1 = temp_databuf[4];
			  if(RecvState == 0)
				{
					for(i=0; i<temp1; i++)
						temppassword[i] = USART_RX_BUF[i+1];
					for(; i<6; i++)
						temppassword[i] = 0;
					temp2 = JX_CheckUserPassword(temppassword);
					if(temp2)
						temp2 = 1;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
				} else if(RecvState == -1)
				{
					state = state_Nop;
					break;
				} else
				{
					UartNeedToRX = temp_databuf[4] + 2;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
					state = state_JPDO;
          break;					
				}
				delay_ms(10);
				JX_UartSendCmdFrame(state_JudgeResult, state_JPDO, temp2, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_JPDO, temp2, 0, 0);
				state = state_Nop;
				break;
      case state_JPJUO :         /*��ȫģʽ����֤����,����*/
			  RecvState = JX_UartRecvDataFrame();
			  temp1 = temp_databuf[4];
			  if(RecvState == 0)
				{
					for(i=0; i<temp1; i++)
						temppassword[i] = USART_RX_BUF[i+1];
					for(; i<6; i++)
						temppassword[i] = 0;
					tempuuid = 0;                     //
					tempuuid |= temppassword[0]<<24;  //
					tempuuid |= temppassword[1]<<16;  //
					tempuuid |= temppassword[2]<<8;   //
					tempuuid |= temppassword[3];      //
					temp2 = JX_CheckUUID(tempuuid);   //
					if(temp2)
						temp2 = 1;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
				} else if(RecvState == -1)
				{
					state = state_Nop;
					break;
				} else
				{
					UartNeedToRX = temp_databuf[4] + 2;
					JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
					state = state_JPDO;
          break;					
				}
				delay_ms(10);
				JX_UartSendCmdFrame(state_JudgeResult, state_JPDO, temp2, 0, 0);
				RecvState = JX_UartRecvCmdFrame();
				if(RecvState != -1)
					if(USART_RX_BUF[3] == 1)
						JX_UartSendCmdFrame(state_JudgeResult, state_JPDO, temp2, 0, 0);
				//������
				state = state_Nop;
				break;
      case state_H7RstFinish :   /*H7��λ���*/
				for(i=0; i<5; i++)
				{
					RecvState = JX_UartRecvCmdFrame();
					if(RecvState == 0)
					{
						JX_UartSendCmdFrame(state_CrcResponse, 0, 0, 0, 0);
						state = state_FinishRst;
					}
					else if(RecvState == -2)
					{
						JX_UartSendCmdFrame(state_CrcResponse, 0, 1, 0, 0);
						state = state_Nop;
					}
					else
						state = state_Nop;
				}
				break;
      case state_H7RequestSleep :/*H7��������*/
				JX_UartSendCmdFrame(state_JudgeResult, state_H7RequestSleep, 1, 0, 0);
			  RecvState = JX_UartRecvCmdFrame();
			  if(RecvState == 0)
				{
					state = state_Nop;
				}
				else if(RecvState == -1)
				{
					state = state_Nop;
				}
				else
				{
					state = state_H7RequestSleep;
				}
			  break;
			default : 
				state = state_Nop;
				break;
		}
	}
}

 uint8_t JX_Usart_CRC8(uint8_t *message, uint8_t len)
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

void JX_UartSendCmdFrame(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t lengthL, uint8_t lengthH)
{
	USART_RX_STA = 0;
	delay_ms(1);
	USART_TX_BUF[0] = 0x13;
	USART_TX_BUF[1] = cmd;
	USART_TX_BUF[2] = data1;
	USART_TX_BUF[3] = data2;
	USART_TX_BUF[4] = lengthL;
	USART_TX_BUF[5] = lengthH;
	USART_TX_BUF[6] = JX_Usart_CRC8(USART_TX_BUF, 6);
	USART_TX_BUF[7] = '\0';
	HAL_UART_Transmit(&huart1, USART_TX_BUF, 7, 0xFFFF);
	delay_ms(1);
}

int8_t JX_UartRecvCmdFrame(void)
{
	timer_alarm_state = 0;
	JX_TimerSetAlarmTime(2);
	while((USART_RX_STA & 0x8000) == 0)
	{
		if(timer_alarm_state)
		{
			timer_alarm_state = 0;
			return -1;
		}
	}
	if(USART_RX_BUF[6] == JX_Usart_CRC8(USART_RX_BUF, 6))
		return 0;
	else
		return -2;
}

int8_t JX_UartRecvDataFrame(void)
{
	uint8_t crc;
	timer_alarm_state = 0;
	while((USART_RX_STA & 0x8000) == 0)
	{
		if(timer_alarm_state)
		{
			timer_alarm_state = 0;
			UartNeedToRX = 7;
			return -1;
		}
	}
	crc = JX_Usart_CRC8(USART_RX_BUF+1, UartNeedToRX -2);
	if(USART_RX_BUF[UartNeedToRX -1] == crc)
	{
		UartNeedToRX = 7;
		return 0;
	}
	else
	{
		UartNeedToRX = 7;
		return -2;
	}
}


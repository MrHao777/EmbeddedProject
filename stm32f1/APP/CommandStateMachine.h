#ifndef _CommandStateMachine_H
#define _CommandStateMachine_H

#include "stm32f1xx_hal.h"

//�ӻ�ָ��
#define state_JudgeUUID             'a'   //�����ж�uuid  ok
#define state_JudgeUserPwd          'b'   //�����ж��û�����  ok
#define state_JudgeAdmiPwd          'c'   //�����жϹ���Ա���� ok
#define state_SetAdmiPwd            'd'   //���ù���Ա���� ok
#define state_SetUserPwd            'e'   //�����û����� ok
#define state_SetFingerprint        'f'   //ע��ָ�� 
#define state_DelFingerprint        'g'   //ɾ��ָ�� 
#define state_SetVerifMode          'h'   //������֤ģʽbit0(0:����ģʽ 1:��ȫģʽ) 
#define state_SetWarnMode           'i'   //���þ���ģʽbit1(0:δ���� 1:����) 
#define state_JPDO                  'j'   //��ȫģʽ����֤����,������ 
#define state_JPJUO                 'k'   //��ȫģʽ����֤����,���� 
#define state_H7RstFinish           'l'   //H7��λ���
#define state_H7RequestSleep        'm'   //H7��������

//����ָ��
#define state_Nop                   0     //��ָ�� 
#define state_CrcResponse           'A'   //crcӦ�� 
#define state_WeekUpH7              'B'   //����H7  ok
#define state_RequestRst            'C'   //����λ 
#define state_FinishRst             'D'   //��λ��� 
#define state_SetMode               'E'   //��������ģʽ ok
#define state_SendAlarmSig          'F'   //���ͱ����ź� 
#define state_SendOpenTime          'G'   //���Ϳ���ʱ��
#define state_ReceivePermit         'H'   //H7�����������������ź� 
#define state_JudgeResult           'Z'   //����ж� 

void JX_CommandStateMachine(void);
 uint8_t JX_Usart_CRC8(uint8_t *message, uint8_t len);
void JX_UartSendCmdFrame(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t lengthL, uint8_t lengthH);
/* 
������������
0:���ճɹ�
-1:��ʱ
-2:crcʧ��
*/
int8_t JX_UartRecvCmdFrame(void);
/* 
������������
0:���ճɹ�
-1:��ʱ
-2:crcʧ��
*/
int8_t JX_UartRecvDataFrame(void);

#endif


#ifndef _CommandStateMachine_H
#define _CommandStateMachine_H

#include "stm32f1xx_hal.h"

//�ӻ�ָ��
#define state_JudgeUUID             'a'   //�����ж�uuid
#define state_JudgeUserPwd          'b'   //�����ж��û�����
#define state_JudgeAdmiPwd          'c'   //�����жϹ���Ա����
#define state_SetAdmiPwd            'd'   //���ù���Ա����
#define state_SetUserPwd            'e'   //�����û�����
#define state_SetFingerprint        'f'   //ע��ָ��
#define state_DelFingerprint        'g'   //ɾ��ָ��
#define state_SetVerifMode          'h'   //������֤ģʽ(0:����ģʽ 1:��ȫģʽ)
#define state_SetWarnMode           'i'   //���þ���ģʽ(0:δ���� 1:����)
#define state_JPDO                  'j'   //��ȫģʽ����֤����,������
#define state_JPJUO                 'k'   //��ȫģʽ����֤����;���,����
#define state_H7RstFinish           'l'   //H7��λ���
#define state_H7RequestSleep        'm'   //H7��������

//����ָ��
#define state_Nop                   0     //��ָ��
#define state_CrcResponse           'A'   //crcӦ��
#define state_WeekUpH7              'B'   //����H7
#define state_RequestRst            'C'   //����λ
#define state_FinishRst             'D'   //��λ���
#define state_SetMode               'E'   //��������ģʽ
#define state_SendAlarmSig          'F'   //���ͱ����ź�
#define state_SendOpenTime          'G'   //���Ϳ���ʱ��
#define state_ReceivePermit         'H'   //H7�����������������ź�
#define state_JudgeResult           'Z'   //����ж�

void JX_CommandStateMachine(void);
static uint8_t JX_Usart_CRC8(uint8_t *message, uint8_t len);
static void JX_ClearTxBuf(void);

#endif


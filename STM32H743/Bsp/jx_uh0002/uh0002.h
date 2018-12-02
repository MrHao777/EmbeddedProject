#ifndef _uh0002_H
#define _uh0002_H
/*
������������
data0			 		 data1		��ע
	'o'     powerValue		���ű���
	'v'			powerValue		�����ܵ�ǿ����
	't'			powerValue		ǰ��������������δ���
	'l'			powerValue		��������10%����
	'w'			powerValue		����
	'i'			powerValue		��������
	'r'			powerValue		�ָ���������
	's'			powerValue		˯��
	
������ȡ����
data0					 data1		��ע
	'I'			powerValue		��ѯ����״̬
	'M'			powerValue		��ѯ��Ȩģʽ
	
��ѯ����״̬����ֵ
IO : �����ɹ�
IE : ����ʧ��
��ѯ��Ȩģʽ����ֵ
0# : ��������,��Ȩ
0* : ��������,δ��Ȩ
E# : �����쳣,��Ȩ
E* : �����쳣,δ��Ȩ


*/
#include "stm32h7xx_hal.h"

void JX_UH0002_Init(void);
void JX_UH0002_DeInit(void);
//�������ֽ�����
int32_t JX_UH0002_SendMessage(uint8_t *senddata);
//�������ֽ�����
int32_t JX_UH0002_ReceiveMessage(uint8_t *receivedata);

#endif

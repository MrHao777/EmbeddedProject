#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	

#define USART_REC_LEN     15                            //�����������ֽ��� 15
#define EN_USART1_RX 			1		                          //ʹ�ܣ�1��/��ֹ��0������1����
extern UART_HandleTypeDef huart1;                       //UART���	  	
extern uint16_t           USART_RX_STA;         		    //����״̬���	
extern uint8_t            USART_RX_BUF[USART_REC_LEN];  //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint8_t            UartRequestState;             //�ӻ�����������
extern uint8_t            UartNeedToRX;                 //���������ֽ���
	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void USART1_IRQHandler(void);
void EXTI0_IRQHandler(void);

#endif

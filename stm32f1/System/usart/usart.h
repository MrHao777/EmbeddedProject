#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	

#define USART_REC_LEN     15                            //定义最大接收字节数 15
#define EN_USART1_RX 			1		                          //使能（1）/禁止（0）串口1接收
extern UART_HandleTypeDef huart1;                       //UART句柄	  	
extern uint16_t           USART_RX_STA;         		    //接收状态标记	
extern uint8_t            USART_RX_BUF[USART_REC_LEN];  //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint8_t            UartRequestState;             //从机数据请求标记
extern uint8_t            UartNeedToRX;                 //接收期望字节数
	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void USART1_IRQHandler(void);
void EXTI0_IRQHandler(void);

#endif

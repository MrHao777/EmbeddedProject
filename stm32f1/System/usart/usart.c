#include "usart.h"

UART_HandleTypeDef huart1; 

//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os 使用	  
#endif

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR=(u8)ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //如果使能了接收
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
uint16_t    USART_RX_STA=0;                  //接收状态标记
uint8_t     USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
uint8_t     UartRequestState=0;              //从机数据请求标记
uint8_t     UartNeedToRX=7;                  //接收期望字节数


//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound)
{	
	//UART 初始化设置
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart1);					    
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();
  if(huart->Instance==USART1)
  {
    __HAL_RCC_USART1_CLK_ENABLE();
  
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
#if EN_USART1_RX
		__HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);		//开启接收中断
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn,1,1);			//抢占优先级3，子优先级3
#endif	
		
		// 数据请求引脚初始化
		__HAL_RCC_GPIOD_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 2);
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  }
}

//串口1中断服务程序
void USART1_IRQHandler(void)                	
{ 
	u8 Res;
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif
	if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE)!=RESET))  
	{
    HAL_UART_Receive(&huart1,&Res,1,1000); 
		if((USART_RX_STA&0x8000)==0)                          //接收未完成
		{
			USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;             //正常接收
			USART_RX_STA++;
			if(USART_RX_STA == UartNeedToRX)                    //接收到了指定数量的数据
				USART_RX_STA|=0x8000;                             //接收完成了 
			if((USART_RX_STA&0X3FFF)>(USART_REC_LEN-1))         //接收数据错误,重新开始接收	
				USART_RX_STA=0;   
		}   		 
	}
	HAL_UART_IRQHandler(&huart1);	
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
} 

void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
	UartRequestState = 1;
}
#endif	

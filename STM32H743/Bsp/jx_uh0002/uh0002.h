#ifndef _uh0002_H
#define _uh0002_H
/*
主机发送数据
data0			 		 data1		备注
	'o'     powerValue		箱门被打开
	'v'			powerValue		箱体受到强烈震动
	't'			powerValue		前面板三次输入三次错码
	'l'			powerValue		电量低于10%报警
	'w'			powerValue		唤醒
	'i'			powerValue		配网设置
	'r'			powerValue		恢复出厂设置
	's'			powerValue		睡眠
	
主机读取数据
data0					 data1		备注
	'I'			powerValue		查询配网状态
	'M'			powerValue		查询授权模式
	
查询配网状态返回值
IO : 配网成功
IE : 配网失败
查询授权模式返回值
0# : 网络正常,授权
0* : 网络正常,未授权
E# : 网络异常,授权
E* : 网络异常,未授权


*/
#include "stm32h7xx_hal.h"

void JX_UH0002_Init(void);
void JX_UH0002_DeInit(void);
//发送两字节数据
int32_t JX_UH0002_SendMessage(uint8_t *senddata);
//接收两字节数据
int32_t JX_UH0002_ReceiveMessage(uint8_t *receivedata);

#endif

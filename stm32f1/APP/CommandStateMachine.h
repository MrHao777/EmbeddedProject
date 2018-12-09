#ifndef _CommandStateMachine_H
#define _CommandStateMachine_H

#include "stm32f1xx_hal.h"

//从机指令
#define state_JudgeUUID             'a'   //请求判断uuid
#define state_JudgeUserPwd          'b'   //请求判断用户密码
#define state_JudgeAdmiPwd          'c'   //请求判断管理员密码
#define state_SetAdmiPwd            'd'   //设置管理员密码
#define state_SetUserPwd            'e'   //设置用户密码
#define state_SetFingerprint        'f'   //注册指纹
#define state_DelFingerprint        'g'   //删除指纹
#define state_SetVerifMode          'h'   //设置验证模式(0:正常模式 1:安全模式)
#define state_SetWarnMode           'i'   //设置警戒模式(0:未开启 1:开启)
#define state_JPDO                  'j'   //安全模式下验证密码,不开箱
#define state_JPJUO                 'k'   //安全模式下验证密码和静脉,开箱
#define state_H7RstFinish           'l'   //H7复位完成
#define state_H7RequestSleep        'm'   //H7请求休眠

//主机指令
#define state_Nop                   0     //无指令
#define state_CrcResponse           'A'   //crc应答
#define state_WeekUpH7              'B'   //唤醒H7
#define state_RequestRst            'C'   //请求复位
#define state_FinishRst             'D'   //复位完成
#define state_SetMode               'E'   //进入设置模式
#define state_SendAlarmSig          'F'   //发送报警信号
#define state_SendOpenTime          'G'   //发送开门时间
#define state_ReceivePermit         'H'   //H7主动发送数据允许信号
#define state_JudgeResult           'Z'   //结果判断

void JX_CommandStateMachine(void);
static uint8_t JX_Usart_CRC8(uint8_t *message, uint8_t len);
static void JX_ClearTxBuf(void);

#endif


#ifndef __UART_H__
#define __UART_H__

#include "stm32f4xx.h"

/* 中断接收变量：1~10 表示语音模块下发的动作编号。 */
extern volatile unsigned char Usart6_Recv_data;

void Uart6_Init(int baud);

#endif

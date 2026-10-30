#include "uart.h"

volatile unsigned char Usart6_Recv_data = 0; /* USART6 接收到的动作命令 */

/*
	串口6初始化
		PC6	---TX
		PC7 ---RX
		baud： 波特率
*/
void Uart6_Init(int baud)
{		
	//2.打开GPIOC端口的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//3.打开USART6的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	
	//4.选择USART6对应的引脚的复用功能
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);
	
	//5.配置USART6的引脚参数，GPIO复用为串口
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;				//复用模式
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;		//引脚速率
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;			//推挽输出	
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;				//上拉电阻
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6|GPIO_Pin_7;	//引脚编号
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//6.配置USART6的参数
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate 		= baud;										//通信速率 
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;						//数据位8bit
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;							//停止位1bit
	USART_InitStructure.USART_Parity 		= USART_Parity_No;							//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	USART_InitStructure.USART_Mode 			= USART_Mode_Rx | USART_Mode_Tx;			//收发模式
	USART_Init(USART6, &USART_InitStructure);
	
	//7.配置NVIC的参数
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;			//中断编号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级，最高优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//打开中断通道
	NVIC_Init(&NVIC_InitStructure);
	
	//8.设置中断的触发方式  接收到数据触发
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	

	
	//9.使能USART6
	USART_Cmd(USART6, ENABLE);
}




//串口6的中断服务函数
void USART6_IRQHandler(void)
{
	//1.判断USART6是否接收到数据
	if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
	{	
		Usart6_Recv_data =  USART_ReceiveData(USART6); //数据处理
		USART_ClearITPendingBit(USART6,USART_IT_RXNE); //清除中断标志
	}
}

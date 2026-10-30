#include "delay.h"


//用基本定时器设置一个微妙级别的延迟
void Timer6_Init(void)
{
	//1. 使能 TIM6
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //84Mhz
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 10000;				//重载值，不需要。
	TIM_TimeBaseStructure.TIM_Prescaler = 84-1; //84分频 1us计数1次 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;					// 不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	// 递增计数
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);	
}
/*
	微秒延迟函数
		@Us: 要延迟多少微秒
*/
void Delay_Us(uint32_t Us)
{
	TIM6->CNT = 0; //清空计数
	TIM_Cmd(TIM6, ENABLE); //打开定时器6，开始计数
	while(TIM6->CNT < Us);
	TIM6->CNT = 0; //清空计数
	TIM_Cmd(TIM6, DISABLE); //关闭定时器
	
}

/*
毫秒延迟函数
	@Ms: 不能超过798 （2^24 / 21/1000/Ms == 798.9）
*/
void Delay_Ms(uint32_t Ms)
{
	while(Ms--)
	{
		Delay_Us(1000);	
	}
	
}

void Delay_S(uint32_t S)
{
	while(S--)
	{
		Delay_Ms(1000);	
	}
	
}



void delay_us(uint16_t us)
{
	SysTick->CTRL = 0; // 关闭定时器
	SysTick->LOAD = us * 21-1; 
	SysTick->VAL = 0; //可用可不用
	SysTick->CTRL = 1; //开启定时器，选择非中断模式、选择21MHZ作为时钟源
	while ((SysTick->CTRL & 0x00010000)==0);//等待计数完成
	SysTick->CTRL = 0; //关闭定时器
}

void delay_ms(uint16_t ms)
{
	SysTick->CTRL = 0; // 关闭定时器
	SysTick->LOAD = ms * 1000 *  21-1; 
	SysTick->VAL = 0; //可用可不用
	SysTick->CTRL = 1; //开启定时器，选择非中断模式、选择21MHZ作为时钟源
	while ((SysTick->CTRL & 0x00010000)==0);//等待计数完成
	SysTick->CTRL = 0; //关闭定时器
}

void delay_s(uint16_t s)
{
	while(s--)
	{
		delay_ms(500);
		delay_ms(500);
	}
}



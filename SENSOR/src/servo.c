#include "servo.h"


unsigned int Speed = 3; //速度参数，数字越大，摆动速度越慢
	
/*
	输出比较初始化，控制四个舵机
	(左后脚)--->PD12 ==> 定时器4  CH1
	(右后脚)--->PD13 ==> 定时器4  CH2
	(左前脚)--->PD14 ==> 定时器4  CH3
	(右前脚)--->PD15 ==> 定时器4  CH4
*/
void PWM_Init(void)
{
	//1. 使能 TIM4 和 GPIOD的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
	//2 GPIO 和 定时器的基础配置
	//GPIO配置为复用模式(推挽)
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_12 |GPIO_Pin_13 |GPIO_Pin_14 | GPIO_Pin_15;			//引脚编号
	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF; 		//复用模式
	GPIO_InitStructure.GPIO_Speed 		= GPIO_Speed_100MHz;	//输出速率
	GPIO_InitStructure.GPIO_OType 		= GPIO_OType_PP;		//推挽模式
	GPIO_InitStructure.GPIO_PuPd 		= GPIO_PuPd_UP ;		//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	
	//3 将GPIO复用为定时器
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

	//定时器基础配置，主要是设置频率，方便后面的计算
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 2000-1;				//重载值，20ms
	TIM_TimeBaseStructure.TIM_Prescaler = 840-1;	 		// 预分频值 840-1  84MHZ/840 = 100000HZ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;					// 不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	// 递增计数
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	
//3. 输出比较配置
	TIM_OCInitTypeDef  TIM_OCInitStructure;	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//PWM模式1  递增计数 则 CNT < CCR 有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出比较
	TIM_OCInitStructure.TIM_Pulse = 50;							//设置CCR寄存器的初值																				
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//输出极性高
	// CNT<CCR时，输出高电平，大于CRR时输出低电平	
	
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	
	
	TIM_OCInitStructure.TIM_Pulse = 250;							//设置CCR寄存器的初值
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);

//4.使能输出比较预装载寄存器
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
//5.使能自动装载寄存器
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	 
//6.使能定时器
	TIM_Cmd(TIM4, ENABLE);
}

/*
	设置左前脚(Left Front)舵机的占空比
	i范围是 [50,250]
*/
void PWM_LF(unsigned int i) //
{
	TIM_SetCompare3(TIM4,i);
}

/*
	设置右前脚(Right Front)舵机的占空比
	i范围是 [50,250]
*/
void PWM_RF(unsigned int i) //
{
	TIM_SetCompare4(TIM4,i);
}


/*
	设置左后脚(Left behand)舵机的占空比
	i范围是 [50,250]
*/
void PWM_LB(unsigned int i) //
{
	TIM_SetCompare1(TIM4,i);
}


/*
	设置右后脚(Right behand)舵机的占空比
	i范围是 [50,250]
*/
void PWM_RB(unsigned int i) //
{
	TIM_SetCompare2(TIM4,i);
}



void Set_LF_Angle(int Angle) //设计左前方舵机的转动的角度
{
	int i = TIM_GetCapture3(TIM4); //获取当前位置CCR的值
	int j = 50 * Angle/45  + 50; //目标位置的CCR的值
	
	if(Speed == 0)
	{
		PWM_LF(j);
	}
	else
	{
		int k;
		if(j >= i)
		{
			for(k = i; k<=j;k++)
			{
				PWM_LF(k);
				delay_ms(Speed);
			}
		}
		else
		{
			for(k = i; k>=j;k--)
			{
				PWM_LF(k);
				delay_ms(Speed);
			}
		
		}
	}
}

void Set_RF_Angle(int Angle) //设置右前方舵机的转动的角度
{
	int i = TIM_GetCapture4(TIM4); //获取当前位置CCR的值
	int j = 250 - Angle * 50/45; //目标位置的CCR的值
	if(Speed == 0)
	{
		PWM_RF(j);
	}
	else
	{
		int k;
		if(j >= i)
		{
			for(k = i; k<=j;k++)
			{
				PWM_RF(k);
				delay_ms(Speed);
			}
		}
		else
		{
			for(k = i; k>=j;k--)
			{
				PWM_RF(k);
				delay_ms(Speed);
			}
		}
	}
}



void Set_LB_Angle(int Angle) //设置左后方舵机的转动的角度
{
	int i = TIM_GetCapture1(TIM4); //获取当前位置CCR的值
	int j = 50 * Angle/45  + 50; //目标位置的CCR的值
	
	if(Speed == 0)
	{
		PWM_LB(j);
	}
	else
	{
		int k;
		if(j >= i)
		{
			for(k = i; k<=j;k++)
			{
				PWM_LB(k);
				delay_ms(Speed);
			}
		}
		else
		{
			for(k = i; k>=j;k--)
			{
				PWM_LB(k);
				delay_ms(Speed);
			}
		
		}
	}
}



void Set_RB_Angle(int Angle) //设置右后方舵机的转动的角度
{
	int i = TIM_GetCapture2(TIM4); //读取右后舵机所在 TIM4_CH2 的当前 CCR 值
	int j = 250 - Angle * 50/45; //目标位置的CCR的值
	if(Speed == 0)
	{
		PWM_RB(j);
	}
	else
	{
		int k;
		if(j >= i)
		{
			for(k = i; k<=j;k++)
			{
				PWM_RB(k);
				delay_ms(Speed);
			}
		}
		else
		{
			for(k = i; k>=j;k--)
			{
				PWM_RB(k);
				delay_ms(Speed);
			}
		}
	}
}


/*动作设计

舵机位置
	FL			FR
	BL			BR
*/

void Action_Up(void)//起立
{
	int i = Speed; //保存原有的摆动速度
	Speed = 0; //无延迟摆动
	Set_LF_Angle(90);
	Set_RF_Angle(90);
	Set_LB_Angle(90);
	Set_RB_Angle(90);
	Speed = i; //恢复原有摆动速度
}

void Action_Sit(void)//坐下
{
	int i = Speed; //保存原有的摆动速度
	Speed = 0; //无延迟摆动
	
	Set_LF_Angle(90);
	Set_RF_Angle(90);
	Set_LB_Angle(0);
	Set_RB_Angle(0);

	Speed = i; //恢复原有摆动速度
}

void Action_Getdowm(void)//趴下
{
	int i = Speed; //保存原有的摆动速度
	Speed = 0; //无延迟摆动
	
	Set_LF_Angle(0);
	Set_RF_Angle(0);
	Set_LB_Angle(0);
	Set_RB_Angle(0);

	Speed = i; //恢复原有摆动速度
}

void Action_Advance(void)//前进
{
	int i = Speed; //保存原有的摆动速度
	Speed = 3; 
	
	Action_Up();//起立
	
	Set_LF_Angle(45);
	Set_RB_Angle(45);
	
	Set_RF_Angle(135);
	Set_LB_Angle(135);
	
	
	Set_LF_Angle(90);
	Set_RB_Angle(90);
	
	Set_RF_Angle(90);
	Set_LB_Angle(90);
	
	Speed = i; //恢复原有摆动速度

}


void Action_Forward(void)//后退
{
	int i = Speed; //保存原有的摆动速度
	Speed = 3; 
	
	Action_Up();//起立
	
	Set_LF_Angle(135);
	Set_RB_Angle(135);
	
	Set_RF_Angle(45);
	Set_LB_Angle(45);
	
	
	Set_LF_Angle(90);
	Set_RB_Angle(90);
	
	Set_RF_Angle(90);
	Set_LB_Angle(90);
	
	Speed = i; //恢复原有摆动速度

}

/*
 * 同时改变四条腿的目标角度，并在舵机到达后短暂停顿。
 * 前脚和后脚反向摆动，使身体沿前后方向摇晃。
 */
static void Servo_Body_Swing(int lf, int rf, int lb, int rb, uint16_t pause_ms)
{
    Set_LF_Angle(lf);
    Set_RF_Angle(rf);
    Set_LB_Angle(lb);
    Set_RB_Angle(rb);
    delay_ms(pause_ms);
}

/*
 * 身体前后摇摆两次（即扭屁股动作）。
 * 每次完整摇摆包含向前、向后两个极限位置，共执行两轮。
 */
void Action_Rock_Twice(void)
{
    unsigned int oldSpeed = Speed;
    unsigned char cycle;

    Speed = 2;          /* 动作速度略快，但保留平滑过渡 */
    Action_Up();        /* 先回到站立姿态，避免舵机从趴下位置直接冲击 */

    for (cycle = 0; cycle < 2u; cycle++)
    {
        /* 身体向前：前脚后摆、后脚前摆 */
        Servo_Body_Swing(65, 65, 115, 115, 120u);
        /* 身体向后：前脚前摆、后脚后摆 */
        Servo_Body_Swing(115, 115, 65, 65, 120u);
    }

    Action_Up();        /* 结束后回到稳定站立姿态 */
    Speed = oldSpeed;   /* 恢复调用前的舵机速度 */
}

/*
 * 趴下动作：四条腿收回到 0 度位置，使机器人贴地。
 */
void Action_Lie_Down(void)
{
    Action_Getdowm();
}

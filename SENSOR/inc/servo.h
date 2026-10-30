#ifndef __SERVO_H__
#define __SERVO_H__

#include "delay.h"
#include "stm32f4xx.h"
void PWM_Init(void);

void PWM_LF(unsigned int i);
void PWM_RF(unsigned int i);
void PWM_LB(unsigned int i);
void PWM_RB(unsigned int i);


void Set_LF_Angle(int Angle);
void Set_RF_Angle(int Angle);
void Set_LB_Angle(int Angle);
void Set_RB_Angle(int Angle);

void Action_Up(void);//起立
void Action_Sit(void);//坐下
void Action_Getdowm(void);//趴下
void Action_Advance(void);//前进

void Action_Forward(void);//后退
void Action_Rock_Twice(void); /* 身体前后摇摆两次（扭屁股） */
void Action_Lie_Down(void);   /* 趴下动作 */
#endif


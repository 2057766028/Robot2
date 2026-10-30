
#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f4xx.h"

void Delay_Us(uint32_t Us);
void Delay_Ms(uint32_t Ms);
void Delay_S(uint32_t S);


void Timer6_Init(void);

void delay_us(uint16_t us);

void delay_ms(uint16_t ms);

void delay_s(uint16_t s);

#endif



#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f4xx.h"

/*
 * DHT11 使用单总线协议，不是 I2C。
 * 本项目将 DAT 接到开发板 PG5，OLED 的 I2C 仍使用 PG3/PG4。
 * 裸传感器 DAT 与 3.3V 之间建议接 4.7k~10k 上拉电阻。
 */
#define DHT11_PORT  GPIOG
#define DHT11_PIN   GPIO_Pin_5

typedef struct
{
    uint8_t humidity_int;       /* 湿度整数部分，单位 %RH */
    uint8_t humidity_dec;       /* 湿度小数部分 */
    uint8_t temperature_int;    /* 温度整数部分，单位摄氏度 */
    uint8_t temperature_dec;    /* 温度小数部分 */
} DHT11_Data_t;

/* 初始化 DAT 为上拉输入。 */
void DHT11_Init(void);

/* 读取一次温湿度；返回 1 成功，返回 0 表示超时或校验失败。 */
uint8_t DHT11_Read(DHT11_Data_t *data);

#endif

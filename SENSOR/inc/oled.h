#ifndef __OLED_H
#define __OLED_H

#include "stm32f4xx.h"

/* GM009605 v3.4 1.9 英寸 OLED，SSD1306/SSD1315，软件 I2C 引脚。 */
#define OLED_SDA_PORT GPIOG
#define OLED_SDA_PIN  GPIO_Pin_3
#define OLED_SCL_PORT GPIOG
#define OLED_SCL_PIN  GPIO_Pin_4

#define OLED_SCL_Clr() GPIO_ResetBits(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SCL_Set() GPIO_SetBits(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SDA_Clr() GPIO_ResetBits(OLED_SDA_PORT, OLED_SDA_PIN)
#define OLED_SDA_Set() GPIO_SetBits(OLED_SDA_PORT, OLED_SDA_PIN)

#define OLED_CMD  0u
#define OLED_DATA 1u

void OLED_Init(void);

/* 初始化后可查询实际探测到的地址和 ACK 状态。 */
uint8_t OLED_GetI2CAddress(void);
uint8_t OLED_IsReady(void);

void OLED_ClearPoint(uint8_t x, uint8_t y);
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
uint8_t OLED_I2C_WriteByte(uint8_t data);
uint8_t OLED_WR_Byte(uint8_t data, uint8_t mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
void OLED_ShowChar(uint8_t x, uint8_t y, char ch, uint8_t color);
void OLED_ShowString(uint8_t x, uint8_t y, const char *text, uint8_t color);
void OLED_ShowPicture(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey,
                      const uint8_t BMP[], uint8_t mode);

#endif

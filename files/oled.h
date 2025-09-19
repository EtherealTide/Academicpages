//
// Created by EtherealTide on 2025/7/11.
//
#ifndef __OLED_H
#define __OLED_H

#include "main.h"

// 屏幕类型枚举
typedef enum
{
    OLED_128x64_096, // 0.96英寸 128x64
    OLED_128x32_091, // 0.91英寸 128x32
    OLED_128x64_13,  // 1.3英寸 128x64
    OLED_128x64_154  // 1.54英寸 128x128
} OLED_Type_t;

// 基本函数
void OLED_Init(void);
void OLED_Update(void);
void OLED_Clear(void);
void OLED_ShowChar(int16_t X, int16_t Y, char Char);
void OLED_ShowString(int16_t X, int16_t Y, char *String);
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length);
void OLED_ClearArea(int16_t X, int16_t Y, int16_t Width, int16_t Height);
// 函数声明
uint8_t OLED_Init_Auto(OLED_Type_t oled_type);
uint8_t OLED_Init_Auto_WithAddr(OLED_Type_t oled_type, uint8_t i2c_addr);
uint8_t OLED_Init_Custom(uint8_t width, uint8_t height, uint8_t com_pins, uint8_t contrast, uint8_t i2c_addr);
void OLED_GetInfo(uint8_t *width, uint8_t *height, uint8_t *pages);
uint8_t OLED_GetI2CAddr(void);
void OLED_SetI2CAddr(uint8_t i2c_addr);

#endif
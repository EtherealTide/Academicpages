//
// Created by EtherealTide on 2025/7/10.
//
#ifndef KEY_H
#define KEY_H

#include "stm32f1xx_hal.h" // change this according to your MCU series, this is for STM32F103C8T6

#define KEY_NUM 6

// 按键状态标志位定义
#define KEY_HOLD 0x01
#define KEY_DOWN 0x02
#define KEY_UP 0x04
#define KEY_SINGLE 0x08
#define KEY_DOUBLE 0x10
#define KEY_LONG 0x20

// 时间阈值定义（单位：ms，基于1ms定时器调用）
#define DEBOUNCE_TIME 20 // 消抖时间 20ms
#define DOUBLE_TIME 300  // 双击检测时间 300ms
#define LONG_TIME 1000   // 长按检测时间 1000ms

// 函数声明
uint8_t get_key_state(uint8_t key_index);
uint8_t check_key_state(uint8_t flag, uint8_t key_index);
void key_process(void);
void key_process_state_machine(uint8_t *S, uint16_t *time, uint8_t *curr_state, uint8_t key_index);

#endif // KEY_H
---
title: "OLED显示屏驱动使用说明（Created by EtherealTide）"
date: 2025-07-15
permalink: /posts/2025/07/oled-driver/
tags:
  - 全国大学生电子设计竞赛
  - IIC
  - 嵌入式开发
  - MCU
  - OLED
categories:
  - Technical Documentation
excerpt: "ET-OLED实现了单片机端的OLED驱动程序，附源代码，使用IIC协议，支持显示数字、字符串、字符。详细介绍了接口使用和完整的程序流程。"
collection: posts
---
- **Download source code**: 
  - [ET_oled-driver.c]({{ site.url }}{{ site.baseurl }}/files/oled.c){:download="oled.c"}
  - [ET_oled-driver.h]({{ site.url }}{{ site.baseurl }}/files/oled.h){:download="oled.h"}

<!-- 保留你的原始内容 -->

## 支持的屏幕型号

- **0.96英寸 128x64** (OLED_128x64_096)
- **0.91英寸 128x32** (OLED_128x32_091)  
- **1.3英寸 128x64** (OLED_128x64_13)
- **1.54英寸 128x64** (OLED_128x64_154)


## 硬件连接

### I2C接口连接（需确保屏幕SCL和SDA自带上拉电阻）

| OLED引脚 | STM32引脚 | 说明      |
| -------- | --------- | --------- |
| VCC      | 3.3V/5V   | 电源正极  |
| GND      | GND       | 电源负极  |
| SCL      | I2C_SCL   | I2C时钟线 |
| SDA      | I2C_SDA   | I2C数据线 |


- **0x78** (0x3C << 1) - 默认地址


## 软件配置

确保STM32CubeMX中I2C配置正确：

- 速度：100-400kHz
- 地址长度：7位


## 用户接口

### 初始化函数

#### 1. 自动配置初始化

```c
uint8_t OLED_Init_Auto(OLED_Type_t oled_type);
```

**参数：**

- `oled_type`: 屏幕类型枚举值

**返回值：**

- `1`: 初始化成功
- `0`: 初始化失败

**示例：**

```c
// 初始化0.96英寸屏幕
OLED_Init_Auto(OLED_128x64_096);
```

#### 2. 指定I2C地址初始化

```c
uint8_t OLED_Init_Auto_WithAddr(OLED_Type_t oled_type, uint8_t i2c_addr);
```

**参数：**

- `oled_type`: 屏幕类型
- `i2c_addr`: 设备的I2C地址（如0x78或0x7A）

**示例：**

```c
// 使用0x7A地址初始化
OLED_Init_Auto_WithAddr(OLED_128x64_096, 0x7A);
```

#### 3. 自定义参数初始化

```c
uint8_t OLED_Init_Custom(uint8_t width, uint8_t height, uint8_t com_pins, 
                        uint8_t contrast, uint8_t i2c_addr);
```

**参数：**

- `width`: 屏幕宽度（像素）
- `height`: 屏幕高度（像素）
- `com_pins`: COM引脚配置
- `contrast`: 对比度设置
- `i2c_addr`: I2C地址

#### 4. 兼容性初始化

```c
void OLED_Init(void);
```

默认初始化0.96英寸屏幕，地址0x78。

### 显示函数

#### 1. 显示字符

```c
void OLED_ShowChar(int16_t X, int16_t Y, char Char);
```

**参数：**

- `X`: 横坐标（0-127）
- `Y`: 纵坐标（0-63）
- `Char`: 要显示的字符

**示例：**

```c
OLED_ShowChar(0, 0, 'A');  // 在(0,0)位置显示字符'A'
```

#### 2. 显示字符串

```c
void OLED_ShowString(int16_t X, int16_t Y, char *String);
```

**参数：**

- `X`: 起始横坐标
- `Y`: 起始纵坐标
- `String`: 字符串指针

**示例：**

```c
OLED_ShowString(0, 0, "Hello OLED!");
```

#### 3. 显示数字

```c
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length);
```

**参数：**

- `X`: 起始横坐标
- `Y`: 起始纵坐标
- `Number`: 要显示的数字
- `Length`: 显示长度（位数）

**示例：**

```c
OLED_ShowNum(0, 16, 12345, 5);  // 显示5位数字
```

### 屏幕控制函数

#### 1. 更新显示

```c
void OLED_Update(void);
```

将显存内容更新到屏幕上。**必须调用此函数才能看到显示效果。**

#### 2. 清除屏幕

```c
void OLED_Clear(void);
```

清空整个显存缓冲区。

#### 3. 清除指定区域

```c
void OLED_ClearArea(int16_t X, int16_t Y, int16_t Width, int16_t Height);
```

**参数：**

- `X`: 起始横坐标
- `Y`: 起始纵坐标
- `Width`: 清除宽度
- `Height`: 清除高度

### 配置函数

#### 1. 获取屏幕信息

```c
void OLED_GetInfo(uint8_t *width, uint8_t *height, uint8_t *pages);
```

#### 2. 获取I2C地址

```c
uint8_t OLED_GetI2CAddr(void);
```

#### 3. 设置I2C地址

```c
void OLED_SetI2CAddr(uint8_t i2c_addr);
```

## 使用示例

### 基本使用示例

```c
#include "oled.h"

int main(void)
{
    // 系统初始化
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    
    // 初始化OLED
    OLED_Init_Auto(OLED_128x64_096);
    
    // 显示内容
    OLED_ShowString(0, 0, "Hello World!");
    OLED_ShowString(0, 16, "OLED Driver");
    OLED_ShowNum(0, 32, 12345, 5);
    
    // 更新显示
    OLED_Update();
    
    while(1)
    {
        // 主循环
    }
}
```

### 动态显示示例

```c
int counter = 0;

while(1)
{
    // 显示静态内容
    OLED_ShowString(0, 0, "Counter:");
    
    // 显示动态数字
    OLED_ShowNum(54, 0, counter, 3);
    
    // 更新显示
    OLED_Update();
    
    // 增加计数器
    counter++;
    if(counter > 999) counter = 0;
    
    HAL_Delay(100);
}
```

### 多屏幕支持示例

```c
// 初始化两个不同地址的屏幕
OLED_Init_Auto_WithAddr(OLED_128x64_096, 0x78);  // 第一个屏幕
// 切换I2C地址
OLED_SetI2CAddr(0x7A);
// 在第二个屏幕上显示
OLED_ShowString(0, 0, "Screen 2");
OLED_Update();
```

## 显示程序流程

### 1. 完整显示流程

```
主循环 while(1)
用户调用显示函数 OLED_ShowString(0, 0, "Hello")
        ↓
显示函数处理流程
        ↓
┌─────────────────────────────────────────────────────────────┐
│                    字符串显示处理                             │
│                                                             │
│  1. 参数验证                                                 │
│     检查坐标范围: X <= (width-6), Y <= (height-8)            │
│     检查字符串指针: String != NULL                           │
│                    ↓                                        │
│  2. 计算字符串长度                                            │
│     遍历字符串: while(String[i] != '\0')                     │
│     计算显示宽度: str_len * 6 像素                           │
│                    ↓                                        │
│  3. 清除显示区域                                              │
│     调用 OLED_ClearArea(X, Y, str_len*6, 8)                │
│     清除6×8像素区域: 防止字符重叠                             │
│                    ↓                                        │
│  4. 逐个字符处理                                              │
│     for(i=0; String[i]!='\0'; i++)                         │
│     调用 OLED_ShowChar(X+i*6, Y, String[i])                │
└─────────────────────────────────────────────────────────────┘
        ↓
单个字符显示流程
        ↓
┌─────────────────────────────────────────────────────────────┐
│                    字符显示处理                               │
│                                                             │
│  1. 字符编码转换                                              │
│     ASCII字符 'A' → 字符索引 ('A' - ' ') = 33               │
│     获取字模数据: OLED_F6x8[33]                             │
│                    ↓                                        │
│  2. 显存坐标计算                                              │
│     页地址 = Y / 8                                          │
│     列地址 = X                                              │
│     位位置 = Y % 8                                          │
│                    ↓                                        │
│  3. 清除字符区域                                              │
│     for(i=0; i<6; i++) // 6列                              │
│       for(j=0; j<8; j++) // 8行                            │
│         OLED_DisplayBuf[page][col] &= ~(1<<bit_pos)        │
│                    ↓                                        │
│  4. 写入新字符                                                │
│     for(i=0; i<6; i++) // 遍历6列字模数据                   │
│       FontData = OLED_F6x8[char_index][i]                  │
│       for(j=0; j<8; j++) // 遍历8位                        │
│         if(FontData & (1<<j))                               │
│           OLED_DisplayBuf[page][col] |= (1<<bit_pos)       │
└─────────────────────────────────────────────────────────────┘
        ↓
用户调用 OLED_Update() 更新显示
        ↓
┌─────────────────────────────────────────────────────────────┐
│                    屏幕更新流程                               │
│                                                             │
│  1. 页面遍历                                                 │
│     for(page=0; page<oled_config.pages; page++)            │
│     设置当前页面地址                                          │
│                    ↓                                        │
│  2. 设置显示位置                                              │
│     OLED_SetCursor(page, 0)                                │
│     发送命令: 0xB0|page, 0x00, 0x10                        │
│                    ↓                                        │
│  3. 发送页面数据                                              │
│     OLED_WriteData(OLED_DisplayBuf[page], width)           │
│     I2C传输: 0x40 + 128字节显存数据                         │
│                    ↓                                        │
│  4. 重复直到完成                                              │
│     处理所有页面 (0~7页，共8页)                              │
│     每页传输129字节 (1控制字节 + 128数据字节)                 │
└─────────────────────────────────────────────────────────────┘
        ↓
I2C底层传输流程
        ↓
┌─────────────────────────────────────────────────────────────┐
│                    I2C通信流程                               │
│                                                             │
│  1. 命令传输                                                 │
│     I2C_Start → 设备地址(0x78) → 控制字节(0x00) → 命令数据   │
│     HAL_I2C_Master_Transmit(&hi2c2, 0x78, cmd_data, 2)    │
│                    ↓                                        │
│  2. 数据传输                                                 │
│     I2C_Start → 设备地址(0x78) → 控制字节(0x40) → 显存数据   │
│     HAL_I2C_Master_Transmit(&hi2c2, 0x78, buffer, 129)    │
│                    ↓                                        │
│  3. 时序控制                                                 │
│     每字节传输: 9位 × 2.5μs = 22.5μs (400kHz)               │
│     每页传输: 129字节 × 22.5μs ≈ 2.9ms                      │
│     全屏更新: 8页 × 2.9ms ≈ 23ms                            │
│                    ↓                                        │
│  4. 硬件响应                                                 │
│     SSD1306控制器接收数据                                    │
│     更新OLED像素点显示                                       │
│     用户看到屏幕内容更新                                      │
└─────────────────────────────────────────────────────────────┘
        ↓
屏幕显示更新完成
        ↓
┌─────────────────┐     ┌─────────────────┐
│   继续主循环     │     │   等待下次调用    │
│                │     │                │
│ while(1) 继续   │     │ 显存保持当前状态  │
│   执行其他代码   │     │   准备接收新的    │
│                │     │   显示指令       │
│  可以继续调用其他 │     │                │
│  OLED显示函数   │     │                │
└─────────────────┘     └─────────────────┘
```

### 2. 数字显示专用流程

```
用户调用 OLED_ShowNum(0, 16, 12345, 5)
        ↓
┌─────────────────────────────────────────────────────────────┐
│                    数字显示处理                               │
│                                                             │
│  1. 清除显示区域                                              │
│     OLED_ClearArea(0, 16, 5*6, 8)                          │
│     清除30×8像素区域 (5位数字)                               │
│                    ↓                                        │
│  2. 计算权重                                                 │
│     temp = 10^(Length-1) = 10^4 = 10000                    │
│     用于提取最高位数字                                        │
│                    ↓                                        │
│  3. 逐位提取数字                                              │
│     for(i=0; i<5; i++)                                      │
│       digit = (12345 / 10000) % 10 = 1                     │
│       OLED_ShowChar(0+i*6, 16, '1')                        │
│       temp /= 10  // 权重递减                               │
│                    ↓                                        │
│  4. 数字转字符                                                │
│     数字1 → ASCII字符'1' (digit + '0')                      │
│     调用字符显示流程                                          │
│     重复处理: 1→2→3→4→5                                      │
└─────────────────────────────────────────────────────────────┘
        ↓
最终显示结果: "12345"
```

### 3. 清除操作流程

```
用户调用 OLED_Clear() 或 OLED_ClearArea()
        ↓
┌─────────────────────────────────────────────────────────────┐
│                    清除操作处理                               │
│                                                             │
│  1. 全屏清除 (OLED_Clear)                                   │
│     memset(OLED_DisplayBuf, 0, pages*width)                │
│     将整个显存缓冲区置零                                      │
│                    ↓                                        │
│  2. 区域清除 (OLED_ClearArea)                              │
│     边界检查: X+Width <= oled_config.width                  │
│     边界检查: Y+Height <= oled_config.height                │
│                    ↓                                        │
│  3. 像素级清除                                                │
│     for(i=0; i<Width; i++)                                  │
│       for(j=0; j<Height; j++)                               │
│         page = (Y+j)/8                                      │
│         bit_pos = (Y+j)%8                                   │
│         OLED_DisplayBuf[page][X+i] &= ~(1<<bit_pos)        │
│                    ↓                                        │
│  4. 等待更新                                                 │
│     显存已清除，等待OLED_Update()更新到屏幕                   │
└─────────────────────────────────────────────────────────────┘
```

## 注意事项

### 1. 显示刷新

- 所有显示函数只是修改显存，必须调用`OLED_Update()`才能看到效果，建议在一次显示更新完成后统一调用`OLED_Update()`

### 2. 坐标系统

- 坐标原点(0,0)在左上角
- X轴向右递增，Y轴向下递增
- 字符大小为6x8像素

### 3. 内存管理

- 显存缓冲区为静态分配，最大支持128x64分辨率
- 不同屏幕尺寸会自动调整使用的显存大小

### 4. I2C地址检测

```c
// 检测设备是否存在
HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, 0x78, 3, 1000);
if(status == HAL_OK) {
    // 设备检测成功
}
```

## 常见问题解决

### 1. 屏幕无显示

- 检查I2C连接是否正确
- 检查电源供应是否正常
- 确认调用了`OLED_Update()`函数

### 2. 如果显示内容重叠（理论上不会）

- 使用`OLED_ClearArea()`清除指定区域
- 或在显示前调用`OLED_Clear()`清除整个屏幕

### 3. 编译错误

- 确保包含了正确的头文件（i2c.h）
- 确保i2c编号正确（程序使用的是hi2c2）

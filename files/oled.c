//
// Created by EtherealTide on 2025/7/11.
//
#include "oled.h"
#include "i2c.h"
#include <string.h>

// OLED屏幕参数结构体
typedef struct
{
  uint8_t width;     // 屏幕宽度
  uint8_t height;    // 屏幕高度
  uint8_t pages;     // 页数 (height/8)
  uint8_t multiplex; // 多路复用率 (height-1)
  uint8_t com_pins;  // COM引脚配置
  uint8_t contrast;  // 对比度
  uint8_t i2c_addr;  // I2C地址
} OLED_Config_t;

// 当前OLED配置
static OLED_Config_t oled_config;

// 动态显存数组指针
static uint8_t (*OLED_DisplayBuf)[128] = NULL;

// 预定义的常见屏幕配置
static const OLED_Config_t OLED_Configs[] = {
    // 0.96英寸 128x64 (默认地址0x78)
    {128, 64, 8, 0x3F, 0x12, 0xCF, 0x78},
    // 0.91英寸 128x32 (默认地址0x78)
    {128, 32, 4, 0x1F, 0x02, 0x8F, 0x78},
    // 1.3英寸 128x64 (默认地址0x78)
    {128, 64, 8, 0x3F, 0x12, 0xFF, 0x78},
    // 1.54英寸 128x64 (默认地址0x78)
    {128, 64, 8, 0x3F, 0x12, 0xFF, 0x78}};

// 精简字符字模 - 6x8点阵
const uint8_t OLED_F6x8[][6] = {
    // ...existing code...
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 空格 (32)
    {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00}, // ! (33)
    {0x00, 0x07, 0x00, 0x07, 0x00, 0x00}, // " (34)
    {0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00}, // # (35)
    {0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00}, // $ (36)
    {0x23, 0x13, 0x08, 0x64, 0x62, 0x00}, // % (37)
    {0x36, 0x49, 0x56, 0x20, 0x50, 0x00}, // & (38)
    {0x00, 0x00, 0x07, 0x00, 0x00, 0x00}, // ' (39)
    {0x00, 0x1C, 0x22, 0x41, 0x00, 0x00}, // ( (40)
    {0x00, 0x41, 0x22, 0x1C, 0x00, 0x00}, // ) (41)
    {0x14, 0x08, 0x3E, 0x08, 0x14, 0x00}, // * (42)
    {0x08, 0x08, 0x3E, 0x08, 0x08, 0x00}, // + (43)
    {0x00, 0x50, 0x30, 0x00, 0x00, 0x00}, // , (44)
    {0x08, 0x08, 0x08, 0x08, 0x08, 0x00}, // - (45)
    {0x00, 0x30, 0x30, 0x00, 0x00, 0x00}, // . (46)
    {0x20, 0x10, 0x08, 0x04, 0x02, 0x00}, // / (47)
    {0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00}, // 0 (48)
    {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00}, // 1 (49)
    {0x42, 0x61, 0x51, 0x49, 0x46, 0x00}, // 2 (50)
    {0x21, 0x41, 0x45, 0x4B, 0x31, 0x00}, // 3 (51)
    {0x18, 0x14, 0x12, 0x7F, 0x10, 0x00}, // 4 (52)
    {0x27, 0x45, 0x45, 0x45, 0x39, 0x00}, // 5 (53)
    {0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00}, // 6 (54)
    {0x01, 0x71, 0x09, 0x05, 0x03, 0x00}, // 7 (55)
    {0x36, 0x49, 0x49, 0x49, 0x36, 0x00}, // 8 (56)
    {0x06, 0x49, 0x49, 0x29, 0x1E, 0x00}, // 9 (57)
    {0x00, 0x36, 0x36, 0x00, 0x00, 0x00}, // : (58)
    {0x00, 0x56, 0x36, 0x00, 0x00, 0x00}, // ; (59)
    {0x08, 0x14, 0x22, 0x41, 0x00, 0x00}, // < (60)
    {0x14, 0x14, 0x14, 0x14, 0x14, 0x00}, // = (61)
    {0x00, 0x41, 0x22, 0x14, 0x08, 0x00}, // > (62)
    {0x02, 0x01, 0x51, 0x09, 0x06, 0x00}, // ? (63)
    {0x32, 0x49, 0x79, 0x41, 0x3E, 0x00}, // @ (64)
    {0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00}, // A (65)
    {0x7F, 0x49, 0x49, 0x49, 0x36, 0x00}, // B (66)
    {0x3E, 0x41, 0x41, 0x41, 0x22, 0x00}, // C (67)
    {0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00}, // D (68)
    {0x7F, 0x49, 0x49, 0x49, 0x41, 0x00}, // E (69)
    {0x7F, 0x09, 0x09, 0x09, 0x01, 0x00}, // F (70)
    {0x3E, 0x41, 0x49, 0x49, 0x7A, 0x00}, // G (71)
    {0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00}, // H (72)
    {0x00, 0x41, 0x7F, 0x41, 0x00, 0x00}, // I (73)
    {0x20, 0x40, 0x41, 0x3F, 0x01, 0x00}, // J (74)
    {0x7F, 0x08, 0x14, 0x22, 0x41, 0x00}, // K (75)
    {0x7F, 0x40, 0x40, 0x40, 0x40, 0x00}, // L (76)
    {0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00}, // M (77)
    {0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00}, // N (78)
    {0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00}, // O (79)
    {0x7F, 0x09, 0x09, 0x09, 0x06, 0x00}, // P (80)
    {0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00}, // Q (81)
    {0x7F, 0x09, 0x19, 0x29, 0x46, 0x00}, // R (82)
    {0x46, 0x49, 0x49, 0x49, 0x31, 0x00}, // S (83)
    {0x01, 0x01, 0x7F, 0x01, 0x01, 0x00}, // T (84)
    {0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00}, // U (85)
    {0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00}, // V (86)
    {0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00}, // W (87)
    {0x63, 0x14, 0x08, 0x14, 0x63, 0x00}, // X (88)
    {0x07, 0x08, 0x70, 0x08, 0x07, 0x00}, // Y (89)
    {0x61, 0x51, 0x49, 0x45, 0x43, 0x00}, // Z (90)
};

// I2C通信函数 - 使用配置的I2C地址
void OLED_WriteCommand(uint8_t Command)
{
  uint8_t data[2];
  data[0] = 0x00;
  data[1] = Command;
  HAL_I2C_Master_Transmit(&hi2c2, oled_config.i2c_addr, data, 2, HAL_MAX_DELAY);
}

// 初始化OLED屏幕 - 根据类型自动配置
uint8_t OLED_Init_Auto(OLED_Type_t oled_type)
{
  if (oled_type >= sizeof(OLED_Configs) / sizeof(OLED_Configs[0]))
  {
    return 0; // 无效类型
  }

  // 复制配置
  oled_config = OLED_Configs[oled_type];

  // 分配显存
  static uint8_t display_buffer[8][128];
  OLED_DisplayBuf = display_buffer;

  HAL_Delay(100);

  // 基本初始化序列
  OLED_WriteCommand(0xAE); // 关闭显示
  OLED_WriteCommand(0xD5); // 设置显示时钟分频比/振荡器频率
  OLED_WriteCommand(0x80);
  OLED_WriteCommand(0xA8); // 设置多路复用率
  OLED_WriteCommand(oled_config.multiplex);
  OLED_WriteCommand(0xD3); // 设置显示偏移
  OLED_WriteCommand(0x00);
  OLED_WriteCommand(0x40); // 设置显示开始行
  OLED_WriteCommand(0xA1); // 设置左右方向
  OLED_WriteCommand(0xC8); // 设置上下方向
  OLED_WriteCommand(0xDA); // 设置COM引脚硬件配置
  OLED_WriteCommand(oled_config.com_pins);
  OLED_WriteCommand(0x81); // 设置对比度
  OLED_WriteCommand(oled_config.contrast);
  OLED_WriteCommand(0xD9); // 设置预充电周期
  OLED_WriteCommand(0xF1);
  OLED_WriteCommand(0xDB); // 设置VCOMH取消选择级别
  OLED_WriteCommand(0x30);
  OLED_WriteCommand(0xA4); // 设置整个显示打开/关闭
  OLED_WriteCommand(0xA6); // 设置正常/反色显示
  OLED_WriteCommand(0x8D); // 设置充电泵
  OLED_WriteCommand(0x14);
  OLED_WriteCommand(0xAF); // 开启显示

  OLED_Clear();
  OLED_Update();

  return 1; // 成功
}

// 初始化OLED屏幕 - 根据类型自动配置，可指定I2C地址
uint8_t OLED_Init_Auto_WithAddr(OLED_Type_t oled_type, uint8_t i2c_addr)
{
  if (oled_type >= sizeof(OLED_Configs) / sizeof(OLED_Configs[0]))
  {
    return 0; // 无效类型
  }

  // 复制配置
  oled_config = OLED_Configs[oled_type];
  // 设置自定义I2C地址
  oled_config.i2c_addr = i2c_addr;

  // 分配显存
  static uint8_t display_buffer[8][128];
  OLED_DisplayBuf = display_buffer;

  HAL_Delay(100);

  // 基本初始化序列
  OLED_WriteCommand(0xAE); // 关闭显示
  OLED_WriteCommand(0xD5); // 设置显示时钟分频比/振荡器频率
  OLED_WriteCommand(0x80);
  OLED_WriteCommand(0xA8); // 设置多路复用率
  OLED_WriteCommand(oled_config.multiplex);
  OLED_WriteCommand(0xD3); // 设置显示偏移
  OLED_WriteCommand(0x00);
  OLED_WriteCommand(0x40); // 设置显示开始行
  OLED_WriteCommand(0xA1); // 设置左右方向
  OLED_WriteCommand(0xC8); // 设置上下方向
  OLED_WriteCommand(0xDA); // 设置COM引脚硬件配置
  OLED_WriteCommand(oled_config.com_pins);
  OLED_WriteCommand(0x81); // 设置对比度
  OLED_WriteCommand(oled_config.contrast);
  OLED_WriteCommand(0xD9); // 设置预充电周期
  OLED_WriteCommand(0xF1);
  OLED_WriteCommand(0xDB); // 设置VCOMH取消选择级别
  OLED_WriteCommand(0x30);
  OLED_WriteCommand(0xA4); // 设置整个显示打开/关闭
  OLED_WriteCommand(0xA6); // 设置正常/反色显示
  OLED_WriteCommand(0x8D); // 设置充电泵
  OLED_WriteCommand(0x14);
  OLED_WriteCommand(0xAF); // 开启显示

  OLED_Clear();
  OLED_Update();

  return 1; // 成功
}

// 自定义初始化 - 手动设置所有参数
uint8_t OLED_Init_Custom(uint8_t width, uint8_t height, uint8_t com_pins, uint8_t contrast, uint8_t i2c_addr)
{
  if (width > 128 || height > 64 || (height % 8) != 0)
  {
    return 0; // 无效参数
  }

  // 设置自定义配置
  oled_config.width = width;
  oled_config.height = height;
  oled_config.pages = height / 8;
  oled_config.multiplex = height - 1;
  oled_config.com_pins = com_pins;
  oled_config.contrast = contrast;
  oled_config.i2c_addr = i2c_addr;

  // 分配显存
  static uint8_t display_buffer[16][128]; // 修改为支持16页
  OLED_DisplayBuf = display_buffer;

  HAL_Delay(100);

  // 基本初始化序列
  OLED_WriteCommand(0xAE); // 关闭显示
  OLED_WriteCommand(0xD5); // 设置显示时钟分频比/振荡器频率
  OLED_WriteCommand(0x80);
  OLED_WriteCommand(0xA8); // 设置多路复用率
  OLED_WriteCommand(oled_config.multiplex);
  OLED_WriteCommand(0xD3); // 设置显示偏移
  OLED_WriteCommand(0x00);
  OLED_WriteCommand(0x40); // 设置显示开始行
  OLED_WriteCommand(0xA1); // 设置左右方向
  OLED_WriteCommand(0xC8); // 设置上下方向
  OLED_WriteCommand(0xDA); // 设置COM引脚硬件配置
  OLED_WriteCommand(oled_config.com_pins);
  OLED_WriteCommand(0x81); // 设置对比度
  OLED_WriteCommand(oled_config.contrast);
  OLED_WriteCommand(0xD9); // 设置预充电周期
  OLED_WriteCommand(0xF1);
  OLED_WriteCommand(0xDB); // 设置VCOMH取消选择级别
  OLED_WriteCommand(0x30);
  OLED_WriteCommand(0xA4); // 设置整个显示打开/关闭
  OLED_WriteCommand(0xA6); // 设置正常/反色显示
  OLED_WriteCommand(0x8D); // 设置充电泵
  OLED_WriteCommand(0x14);
  OLED_WriteCommand(0xAF); // 开启显示

  OLED_Clear();
  OLED_Update();

  return 1; // 成功
}

// 兼容旧版本的初始化函数
void OLED_Init(void)
{
  OLED_Init_Auto(OLED_128x64_096); // 默认0.96英寸屏幕
}

void OLED_WriteData(uint8_t *Data, uint8_t Count)
{
  uint8_t buffer[129];
  buffer[0] = 0x40;
  for (uint8_t i = 0; i < Count; i++)
  {
    buffer[i + 1] = Data[i];
  }
  HAL_I2C_Master_Transmit(&hi2c2, oled_config.i2c_addr, buffer, Count + 1, HAL_MAX_DELAY);
}

// 设置光标位置
void OLED_SetCursor(uint8_t Page, uint8_t X)
{
  OLED_WriteCommand(0xB0 | Page);
  OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));
  OLED_WriteCommand(0x00 | (X & 0x0F));
}

// 更新显示
void OLED_Update(void)
{
  uint8_t j;
  for (j = 0; j < oled_config.pages; j++)
  {
    OLED_SetCursor(j, 0);
    OLED_WriteData(OLED_DisplayBuf[j], oled_config.width);
  }
}

// 清空显存
void OLED_Clear(void)
{
  memset(OLED_DisplayBuf, 0, oled_config.pages * oled_config.width);
}

// 清除指定区域
void OLED_ClearArea(int16_t X, int16_t Y, int16_t Width, int16_t Height)
{
  uint8_t i, j;

  if (X >= oled_config.width || Y >= oled_config.height)
    return;

  // 限制清除区域在屏幕范围内
  if (X + Width > oled_config.width)
    Width = oled_config.width - X;
  if (Y + Height > oled_config.height)
    Height = oled_config.height - Y;

  for (i = 0; i < Width; i++)
  {
    for (j = 0; j < Height; j++)
    {
      OLED_DisplayBuf[(Y + j) / 8][X + i] &= ~(0x01 << ((Y + j) % 8));
    }
  }
}

// 显示字符 - 使用清除区域功能
void OLED_ShowChar(int16_t X, int16_t Y, char Char)
{
  uint8_t i, j;
  uint8_t FontData;

  if (X > (oled_config.width - 6) || Y > (oled_config.height - 8))
    return; // 超出屏幕范围

  // 先清除这个字符位置
  OLED_ClearArea(X, Y, 6, 8);

  // 再显示新的字符
  for (i = 0; i < 6; i++)
  {
    FontData = OLED_F6x8[Char - ' '][i];
    for (j = 0; j < 8; j++)
    {
      if (FontData & (0x01 << j))
      {
        OLED_DisplayBuf[(Y + j) / 8][X + i] |= 0x01 << ((Y + j) % 8);
      }
    }
  }
}

// 显示字符串 - 先清除整个字符串区域
void OLED_ShowString(int16_t X, int16_t Y, char *String)
{
  uint16_t i = 0;
  uint16_t str_len = 0;

  // 计算字符串长度
  while (String[str_len] != '\0' && (X + str_len * 6) < oled_config.width)
  {
    str_len++;
  }

  // 清除整个字符串区域
  OLED_ClearArea(X, Y, str_len * 6, 8);

  // 显示字符串
  while (String[i] != '\0')
  {
    OLED_ShowChar(X + i * 6, Y, String[i]);
    i++;
    if (X + i * 6 > (oled_config.width - 6))
      break; // 防止超出屏幕
  }
}

// 显示数字 - 先清除数字区域
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length)
{
  uint8_t i;
  uint32_t temp = 1;

  // 先清除数字显示区域
  OLED_ClearArea(X, Y, Length * 6, 8);

  // 计算10的(Length-1)次方
  for (i = 1; i < Length; i++)
  {
    temp *= 10;
  }

  for (i = 0; i < Length; i++)
  {
    OLED_ShowChar(X + i * 6, Y, (Number / temp) % 10 + '0');
    temp /= 10;
  }
}

// 获取当前屏幕信息
void OLED_GetInfo(uint8_t *width, uint8_t *height, uint8_t *pages)
{
  if (width)
    *width = oled_config.width;
  if (height)
    *height = oled_config.height;
  if (pages)
    *pages = oled_config.pages;
}

// 获取当前I2C地址
uint8_t OLED_GetI2CAddr(void)
{
  return oled_config.i2c_addr;
}

// 设置I2C地址 (在运行时修改)
void OLED_SetI2CAddr(uint8_t i2c_addr)
{
  oled_config.i2c_addr = i2c_addr;
}
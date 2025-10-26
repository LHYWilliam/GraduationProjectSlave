#ifndef OLED_FONT_H
#define OLED_FONT_H

#include "stm32f1xx_hal.h"

#define OLED_ChineseBytesCount 3

#define IsChinese(string) ((((string)[0]) & 0x80) != 0x00)

typedef enum
{
  OLEDFont_None,
  OLEDFont_6X8,
  OLEDFont_8X16,
  OLEDFont_Chinese12X12,
} OLEDFont;

extern const uint8_t OLED_Font6x8[][6];
extern const uint8_t OLED_Font8x16[][16];
extern const char OLED_FontChinese12x12_Index[];
extern const uint8_t OLED_FontChinese12x12[][24];
extern const uint8_t SettingImage[][8];
extern const uint8_t MenuImage[][128];

#endif

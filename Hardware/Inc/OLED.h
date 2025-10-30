#ifndef OLED_H
#define OLED_H

#include "stm32f1xx_hal.h"

#include "OLED_Font.h"

typedef struct OLED_t
{
  FlagStatus I2C;
  I2C_HandleTypeDef *hI2Cx;

  GPIO_TypeDef *SCL_Port;
  uint32_t SCL_Pin;
  GPIO_TypeDef *SDA_Port;
  uint32_t SDA_Pin;

  uint8_t Width;
  uint8_t Height;

  OLEDFont Font;
  uint8_t FontWidth;
  uint8_t FontHeight;

  uint8_t CommandsBuffer[3];
  uint8_t PrintfBuffer[128];
  uint8_t DisplayBuffer[8][128];

  void (*OLED_WriteDatas)(struct OLED_t *Self, uint8_t *Datas, uint16_t Length);
  void (*OLED_WriteCommand)(struct OLED_t *Self, uint8_t Command);
  void (*OLED_WriteCommands)(struct OLED_t *Self, uint8_t *Commands, uint16_t Length);
} OLED_t;

void OLED_Init(OLED_t *Self);

void OLED_Fill(OLED_t *Self);
void OLED_Clear(OLED_t *Self);
void OLED_Reverse(OLED_t *Self);
void OLED_FillArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void OLED_ClearArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void OLED_ReverseArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

void OLED_DrawPoint(OLED_t *Self, int16_t X, int16_t Y);
void OLED_DrawHLine(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Step);
void OLED_DrawVLine(OLED_t *Self, int16_t X, int16_t Y, uint8_t Height, uint8_t Step);
void OLED_DrawLine(OLED_t *Self, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2);
void OLED_DrawHollowRectangle(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t Step);
void OLED_DrawSolidRectangle(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void OLED_DrawChart(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint16_t *Data, uint16_t Length, int16_t Index);

void OLED_ShowImage(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image);

void OLED_SetFont(OLED_t *Self, OLEDFont Font);
void OLED_ShowChar(OLED_t *Self, int16_t X, int16_t Y, char Char);
void OLED_ShowString(OLED_t *Self, int16_t X, int16_t Y, const char *String);
void OLED_Printf(OLED_t *Self, int16_t X, int16_t Y, const char *Format, ...);

void OLED_ClearBuffer(OLED_t *Self);
void OLED_SendBuffer(OLED_t *Self);
void OLED_ClearBufferArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void OLED_SendBufferArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

#endif

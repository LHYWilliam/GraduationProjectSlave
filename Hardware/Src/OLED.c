#include <stdarg.h>
#include <stdio.h>

#include "cmsis_os2.h"

#include "OLED.h"
#include "OLED_I2C.h"
#include "OLED_Font.h"


void OLED_ConfigReg(OLED_t *Self);

void OLED_Init(OLED_t *Self)
{
  if (Self->Width == 0)
  {
    Self->Width = 128;
  }
  if (Self->Height == 0)
  {
    Self->Height = 64;
  }

  if (Self->Font == OLEDFont_None)
  {
    OLED_SetFont(Self, OLEDFont_6X8);
  }

  if (Self->hI2Cx)
  {
    Self->OLED_WriteDatas = OLED_I2C_WriteDatas;
    Self->OLED_WriteCommand = OLED_I2C_WriteCommand;
    Self->OLED_WriteCommands = OLED_I2C_WriteCommands;
  } else
  {
    return;
  }

  OLED_ConfigReg(Self);

  OLED_Clear(Self);
}

void OLED_SetCursor(OLED_t *Self, uint8_t Page, uint8_t X)
{
  Self->CommandsBuffer[0] = 0xB0 | Page;
  Self->CommandsBuffer[1] = 0x10 | ((X & 0xF0) >> 4);
  Self->CommandsBuffer[2] = 0x00 | (X & 0x0F);
  Self->OLED_WriteCommands(Self, Self->CommandsBuffer, 3);
}

void OLED_Fill(OLED_t *Self)
{
  for (uint8_t j = 0; j < 8; j++)
  {
    for (uint8_t i = 0; i < Self->Width; i++)
    {
      Self->DisplayBuffer[j][i] = 0xFF;
    }
  }
}

void OLED_FillArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                   uint8_t Height)
{
  for (int16_t j = Y; j < Y + Height; j++)
  {
    for (int16_t i = X; i < X + Width; i++)
    {
      if (i >= 0 && i < Self->Width && j >= 0 && j < Self->Height)
      {
        Self->DisplayBuffer[j / 8][i] |= 0x01 << (j % 8);
      }
    }
  }
}

void OLED_Clear(OLED_t *Self)
{
  for (uint8_t j = 0; j < 8; j++)
  {
    for (uint8_t i = 0; i < Self->Width; i++)
    {
      Self->DisplayBuffer[j][i] = 0x00;
    }
  }
}

void OLED_ClearArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                    uint8_t Height)
{
  for (int16_t j = Y; j < Y + Height; j++)
  {
    for (int16_t i = X; i < X + Width; i++)
    {
      if (i >= 0 && i < Self->Width && j >= 0 && j < Self->Height)
      {
        Self->DisplayBuffer[j / 8][i] &= ~(0x01 << (j % 8));
      }
    }
  }
}

void OLED_Reverse(OLED_t *Self)
{
  for (uint8_t j = 0; j < 8; j++)
  {
    for (uint8_t i = 0; i < Self->Width; i++)
    {
      Self->DisplayBuffer[j][i] ^= 0xFF;
    }
  }
}

void OLED_ReverseArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                      uint8_t Height)
{
  for (int16_t j = Y; j < Y + Height; j++)
  {
    for (int16_t i = X; i < X + Width; i++)
    {
      if (i >= 0 && i < Self->Width && j >= 0 && j < Self->Height)
      {
        Self->DisplayBuffer[j / 8][i] ^= 0x01 << (j % 8);
      }
    }
  }
}

void OLED_DrawPoint(OLED_t *Self, int16_t X, int16_t Y)
{
  if (X >= 0 && X < Self->Width && Y >= 0 && Y < Self->Height)
  {
    Self->DisplayBuffer[Y / 8][X] |= 0x01 << (Y % 8);
  }
}

void OLED_DrawHLine(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                    uint8_t step)
{
  for (uint8_t i = 0; i < Width; i += step)
  {
    OLED_DrawPoint(Self, X + i, Y);
  }
}

void OLED_DrawVLine(OLED_t *Self, int16_t X, int16_t Y, uint8_t Height,
                    uint8_t step)
{
  for (uint8_t i = 0; i < Height; i += step)
  {
    OLED_DrawPoint(Self, X, Y + i);
  }
}

void OLED_DrawLine(OLED_t *Self, int16_t X1, int16_t Y1, int16_t X2,
                   int16_t Y2)
{
  uint16_t dx, dy;
  if (X1 > X2)
  {
    dx = X1 - X2;
  } else
  {
    dx = X2 - X1;
  }
  if (Y1 > Y2)
  {
    dy = Y1 - Y2;
  } else
  {
    dy = Y2 - Y1;
  }

  uint16_t tmp;
  uint8_t swapxy = 0;
  if (dy > dx)
  {
    swapxy = 1;
    tmp = dx;
    dx = dy;
    dy = tmp;
    tmp = X1;
    X1 = Y1;
    Y1 = tmp;
    tmp = X2;
    X2 = Y2;
    Y2 = tmp;
  }
  if (X1 > X2)
  {
    tmp = X1;
    X1 = X2;
    X2 = tmp;
    tmp = Y1;
    Y1 = Y2;
    Y2 = tmp;
  }
  int16_t err = dx >> 1;
  int16_t ystep = Y2 > Y1 ? 1 : -1;

  uint16_t y = Y1;

  if (X2 == 255)
  {
    X2--;
  }

  for (uint16_t x = X1; x <= X2; x++)
  {
    if (swapxy == 0)
    {
      OLED_DrawPoint(Self, x, y);
    } else
    {
      OLED_DrawPoint(Self, y, x);
    }
    err -= (uint16_t) dy;
    if (err < 0)
    {
      y += (uint16_t) ystep;
      err += (uint16_t) dx;
    }
  }
}

void OLED_DrawHollowRectangle(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                              uint8_t Height)
{
  OLED_DrawHLine(Self, X, Y, Width, 1);
  OLED_DrawHLine(Self, X, Y + Height - 1, Width, 1);
  OLED_DrawVLine(Self, X, Y, Height, 1);
  OLED_DrawVLine(Self, X + Width - 1, Y, Height, 1);
}

void OLED_DrawSolidRectangle(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                             uint8_t Height)
{
  OLED_FillArea(Self, X, Y, Width, Height);
}

void OLED_ShowChart(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                    uint8_t Height, uint16_t *Data, uint16_t Length,
                    int16_t Index)
{
  Index = (Index + 1) % Length;
  for (uint8_t x = 0; x < Self->Width - 1;
       x++, Index = (Index + 1) % Length)
  {
    OLED_DrawLine(Self, OLED_IndexToX(x, Length, X, Width),
                  OLED_ADCToY(Data[Index], Y, Height),
                  OLED_IndexToX(x + 1, Length, X, Width),
                  OLED_ADCToY(Data[(Index + 1) % Length], Y, Height));
  }

  OLED_DrawHLine(Self, X, Y, Width, 3);
  OLED_DrawHLine(Self, X, Y + Height - 1, Width, 3);
  OLED_DrawVLine(Self, X, Y, Height, 3);
  OLED_DrawVLine(Self, X + Width - 1, Y, Height, 3);
}

void OLED_ShowImage(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                    uint8_t Height, const uint8_t *Image)
{
  OLED_ClearArea(Self, X, Y, Width, Height);

  for (uint8_t j = 0; j < (Height - 1) / 8 + 1; j++)
  {
    for (uint8_t i = 0; i < Width; i++)
    {
      if (X + i >= 0 && X + i < Self->Width)
      {
        int16_t Page = Y / 8;
        int16_t Shift = Y % 8;
        if (Y < 0)
        {
          Page -= 1;
          Shift += 8;
        }

        if (Page + j >= 0 && Page + j <= 7)
        {
          Self->DisplayBuffer[Page + j][X + i] |= Image[j * Width + i]
                                                  << (Shift);
        }

        if (Page + j + 1 >= 0 && Page + j + 1 <= 7)
        {
          Self->DisplayBuffer[Page + j + 1][X + i] |=
              Image[j * Width + i] >> (8 - Shift);
        }
      }
    }
  }
}

void OLED_SetFont(OLED_t *Self, OLEDFont Font)
{
  Self->Font = Font;

  switch (Self->Font)
  {
  case OLEDFont_6X8:
    Self->FontWidth = 6;
    Self->FontHeight = 8;
    break;

  case OLEDFont_8X16:
    Self->FontWidth = 8;
    Self->FontHeight = 16;
    break;

  case OLEDFont_Chinese12X12:
    Self->FontWidth = 12;
    Self->FontHeight = 12;
    break;

  default:
    break;
  }
}

void OLED_ShowChar(OLED_t *Self, int16_t X, int16_t Y, char Char)
{
  switch (Self->Font)
  {
  case OLEDFont_6X8:
    OLED_ShowImage(Self, X, Y, Self->FontWidth, Self->FontHeight,
                   OLED_Font6x8[Char - ' ']);
    break;

  case OLEDFont_8X16:
    OLED_ShowImage(Self, X, Y, Self->FontWidth, Self->FontHeight,
                   OLED_Font8x16[Char - ' ']);
    break;

  default:
    break;
  }
}

void OLED_ShowString(OLED_t *Self, int16_t X, int16_t Y, const char *String)
{
  for (uint8_t i = 0; String[i];)
  {
    OLED_ShowChar(Self, X, Y, String[i]);
    X += Self->FontWidth;

    i += 1;
  }
}

void OLED_Printf(OLED_t *Self, int16_t X, int16_t Y, const char *Format, ...)
{
  va_list Args;
  va_start(Args, Format);
  vsprintf((char *) Self->PrintfBuffer, Format, Args);
  va_end(Args);

  OLED_ShowString(Self, X, Y, (char *) Self->PrintfBuffer);
}

void OLED_ClearBuffer(OLED_t *Self) { OLED_Clear(Self); }

void OLED_ClearBufferArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                          uint8_t Height)
{
  OLED_ClearArea(Self, X, Y, Width, Height);
}

void OLED_SendBuffer(OLED_t *Self)
{
  for (uint8_t j = 0; j < 8; j++)
  {
    OLED_SetCursor(Self, j, 0);
    Self->OLED_WriteDatas(Self, Self->DisplayBuffer[j], Self->Width);
  }
}

void OLED_SendBufferArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                         uint8_t Height)
{
  int16_t Page = Y / 8;
  int16_t Page1 = (Y + Height - 1) / 8 + 1;
  if (Y < 0)
  {
    Page -= 1;
    Page1 -= 1;
  }

  for (int16_t j = Page; j < Page1; j++)
  {
    if (X >= 0 && X < Self->Width && j >= 0 && j <= 7)
    {

      OLED_SetCursor(Self, j, X);
      Self->OLED_WriteDatas(Self, &Self->DisplayBuffer[j][X], Width);
    }
  }
}

void OLED_ConfigReg(OLED_t *Self)
{
  // if (Self->SPI || Self->SPIx)
  // {
  //   GPIO_Write(Self->RES_ODR, 0);
  // }
  osDelay(100);
  // if (Self->SPI || Self->SPIx)
  // {
  //   GPIO_Write(Self->RES_ODR, 1);
  // }

  Self->OLED_WriteCommand(Self, 0xAE); // 关闭显示

  Self->OLED_WriteCommand(Self, 0xD5); // 设置时钟
  Self->OLED_WriteCommand(Self, 0x80);

  Self->OLED_WriteCommand(Self, 0xA8); // 设置多路复
  Self->OLED_WriteCommand(Self, 0x3F);

  Self->OLED_WriteCommand(Self, 0xD3); // 设置显示偏移
  Self->OLED_WriteCommand(Self, 0x00);

  Self->OLED_WriteCommand(Self, 0x40); // 设置起始行

  Self->OLED_WriteCommand(Self, 0xA1); // 设置左右方向

  Self->OLED_WriteCommand(Self, 0xC8); // 设置上下方向

  Self->OLED_WriteCommand(Self, 0xDA); // 设置COM硬件引脚配置
  Self->OLED_WriteCommand(Self, 0x12);

  Self->OLED_WriteCommand(Self, 0x81); // 调节亮度
  Self->OLED_WriteCommand(Self, 0xCF);

  Self->OLED_WriteCommand(Self, 0xD9); // 设置预充电周期
  Self->OLED_WriteCommand(Self, 0xF1);

  Self->OLED_WriteCommand(Self, 0xDB); // 设置VCOMH
  Self->OLED_WriteCommand(Self, 0x30);

  Self->OLED_WriteCommand(Self, 0xA4); // 全局显示开启

  Self->OLED_WriteCommand(Self, 0xA6); // 设置显示方式

  Self->OLED_WriteCommand(Self, 0x8D); // 设置电荷泵
  Self->OLED_WriteCommand(Self, 0x14);

  Self->OLED_WriteCommand(Self, 0xAF); // 打开显示
}

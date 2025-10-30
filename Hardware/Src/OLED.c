#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_os2.h"

#include "OLED.h"
#include "OLED_I2C.h"
#include "OLED_Font.h"


void OLED_Config(OLED_t *Self);

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

  OLED_Config(Self);

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
  memset(Self->DisplayBuffer, 0xFF, sizeof(Self->DisplayBuffer));
}

void OLED_Clear(OLED_t *Self)
{
  memset(Self->DisplayBuffer, 0x00, sizeof(Self->DisplayBuffer));
}

void OLED_Reverse(OLED_t *Self)
{
  for (uint8_t j = 0; j < Self->Height / 8; j++)
  {
    for (uint8_t i = 0; i < Self->Width; i++)
    {
      Self->DisplayBuffer[j][i] ^= 0xFF;
    }
  }
}

void OLED_FillArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                   uint8_t Height)
{
  if (X >= Self->Width || Y >= Self->Height || X + Width < 0 || Y + Height < 0)
  {
    return;
  }

  uint8_t XBegin = X < 0 ? 0 : X;
  uint8_t XEnd = (X + Width) > Self->Width ? Self->Width : (X + Width);
  uint8_t YBegin = Y < 0 ? 0 : Y;
  uint8_t YEnd = (Y + Height) > Self->Height ? Self->Height : (Y + Height);

  for (int16_t j = YBegin; j < YEnd; j++)
  {
    uint8_t Page = j / 8;
    uint8_t Mask = 0x01 << (j % 8);

    for (int16_t i = XBegin; i < XEnd; i++)
    {
      Self->DisplayBuffer[Page][i] |= Mask;
    }
  }
}


void OLED_ClearArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                    uint8_t Height)
{
  if (X >= Self->Width || Y >= Self->Height || X + Width < 0 || Y + Height < 0)
  {
    return;
  }

  uint8_t XBegin = X < 0 ? 0 : X;
  uint8_t XEnd = (X + Width) > Self->Width ? Self->Width : (X + Width);
  uint8_t YBegin = Y < 0 ? 0 : Y;
  uint8_t YEnd = (Y + Height) > Self->Height ? Self->Height : (Y + Height);

  for (int16_t j = YBegin; j < YEnd; j++)
  {
    uint8_t Page = j / 8;
    uint8_t Mask = ~(0x01 << (j % 8));

    for (int16_t i = XBegin; i < XEnd; i++)
    {
      Self->DisplayBuffer[Page][i] &= Mask;
    }
  }
}

void OLED_ReverseArea(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width,
                      uint8_t Height)
{
  if (X >= Self->Width || Y >= Self->Height || X + Width < 0 || Y + Height < 0)
  {
    return;
  }

  uint8_t XBegin = X < 0 ? 0 : X;
  uint8_t XEnd = (X + Width) > Self->Width ? Self->Width : (X + Width);
  uint8_t YBegin = Y < 0 ? 0 : Y;
  uint8_t YEnd = (Y + Height) > Self->Height ? Self->Height : (Y + Height);

  for (int16_t j = YBegin; j < YEnd; j++)
  {
    uint8_t Page = j / 8;
    uint8_t Mask = 0x01 << (j % 8);

    for (int16_t i = XBegin; i < XEnd; i++)
    {
      Self->DisplayBuffer[Page][i] ^= Mask;
    }
  }
}

void OLED_DrawPoint(OLED_t *Self, int16_t X, int16_t Y)
{
  if (X >= 0 && Y >= 0 && X < Self->Width && Y < Self->Height)
  {
    Self->DisplayBuffer[Y / 8][X] |= 0x01 << (Y % 8);
  }
}

void OLED_DrawHLine(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Step)
{
  if (X >= Self->Width || X + Width < 0 || Y < 0 || Y >= Self->Height)
  {
    return;
  }

  for (uint8_t i = 0; i < Width; i += Step)
  {
    if (X + i < 0)
    {
      continue;
    } else if (X + i >= Self->Width)
    {
      break;
    }

    OLED_DrawPoint(Self, X + i, Y);
  }
}

void OLED_DrawVLine(OLED_t *Self, int16_t X, int16_t Y, uint8_t Height, uint8_t Step)
{
  if (X < 0 || X >= Self->Width || Y >= Self->Height || Y + Height < 0)
  {
    return;
  }

  for (uint8_t i = 0; i < Height; i += Step)
  {
    if (Y + i < 0)
    {
      continue;
    } else if (Y + i >= Self->Height)
    {
      break;
    }


    OLED_DrawPoint(Self, X, Y + i);
  }
}

void OLED_DrawLine(OLED_t *Self, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2)
{
  int16_t dx = (X2 - X1) < 0 ? -(X2 - X1) : (X2 - X1);
  int16_t dy = (Y2 - Y1) < 0 ? -(Y2 - Y1) : (Y2 - Y1);
  int16_t sx = (X1 < X2) ? 1 : -1;
  int16_t sy = (Y1 < Y2) ? 1 : -1;
  int16_t err = dx - dy;
  int16_t err2;

  while (1)
  {
    if (X1 >= 0 && X1 < Self->Width && Y1 >= 0 && Y1 < Self->Height)
    {
      Self->DisplayBuffer[Y1 / 8][X1] |= (1 << (Y1 % 8));
    }

    if (X1 == X2 && Y1 == Y2)
    {
      break;
    }

    err2 = 2 * err;
    if (err2 > -dy)
    {
      err -= dy;
      X1 += sx;
    }
    if (err2 < dx)
    {
      err += dx;
      Y1 += sy;
    }
  }
}

void OLED_DrawHollowRectangle(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t Step)
{
  OLED_DrawHLine(Self, X, Y, Width, Step);
  OLED_DrawHLine(Self, X, Y + Height - 1, Width, Step);
  OLED_DrawVLine(Self, X, Y, Height, Step);
  OLED_DrawVLine(Self, X + Width - 1, Y, Height, Step);
}

void OLED_DrawSolidRectangle(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
  OLED_FillArea(Self, X, Y, Width, Height);
}

#define Normalization(Data, Origin, Target, Offset) ((Data) * Target / Origin + Offset)

void OLED_DrawChart(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint16_t *Data, uint16_t Length, int16_t Index)
{
  for (uint8_t i = 0; i < Length - 1; i++, Index = (Index + 1) % Length)
  {
    OLED_DrawLine(Self,
                  Normalization(i, Length - 1, Width, X),
                  Normalization(4095 - Data[Index], 4095, Height, Y),
                  Normalization(i + 1, Length - 1, Width, X),
                  Normalization(4095 - Data[(Index + 1) % Length], 4095, Height, Y));
  }

  OLED_DrawHollowRectangle(Self, X, Y, Width, Height, 3);
}

// void OLED_ShowImage(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
// {
//   if (X >= Self->Width || Y >= Self->Height || X + Width < 0 || Y + Height < 0)
//   {
//     return;
//   }

//   uint8_t Pages = (Height + 7) / 8;

//   int16_t StartPage = Y / 8;
//   int16_t Offset = Y % 8;

//   for (int16_t Page = 0; Page < Pages; Page++)
//   {
//     int16_t TargetPage = StartPage + Page;
//     if (TargetPage < 0)
//     {
//       continue;
//     } else if (TargetPage >= 8)
//     {
//       break;
//     }

//     for (int16_t i = 0; i < Width; i++)
//     {
//       int16_t TargetX = X + i;
//       if (TargetX < 0)
//       {
//         continue;
//       } else if (TargetX >= Self->Width)
//       {
//         break;
//       }

//       uint8_t ImageData = Image[Page * Width + i];

//       if (Offset == 0)
//       {
//         Self->DisplayBuffer[TargetPage][TargetX] |= ImageData;
//       } else
//       {
//         Self->DisplayBuffer[TargetPage][TargetX] |= ImageData << Offset;
//         Self->DisplayBuffer[TargetPage + 1][TargetX] |= ImageData >> (8 - Offset);
//       }
//     }
//   }
// }

void OLED_ShowImage(OLED_t *Self, int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
  // OLED_ClearArea(Self, X, Y, Width, Height);

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
          Self->DisplayBuffer[Page + j][X + i] |= Image[j * Width + i] << (Shift);
        }

        if (Page + j + 1 >= 0 && Page + j + 1 <= 7)
        {
          Self->DisplayBuffer[Page + j + 1][X + i] |= Image[j * Width + i] >> (8 - Shift);
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
    OLED_ShowImage(Self, X, Y, Self->FontWidth, Self->FontHeight, OLED_Font6x8[Char - ' ']);
    break;

  case OLEDFont_8X16:
    OLED_ShowImage(Self, X, Y, Self->FontWidth, Self->FontHeight, OLED_Font8x16[Char - ' ']);
    break;

  default:
    break;
  }
}

void OLED_ShowString(OLED_t *Self, int16_t X, int16_t Y, const char *String)
{
  for (uint8_t i = 0; String[i]; i++)
  {
    if (X + Self->FontWidth < 0)
    {
      X += Self->FontWidth;
      continue;
    } else if (X >= Self->Width)
    {
      break;
    }

    OLED_ShowChar(Self, X, Y, String[i]);

    X += Self->FontWidth;
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
  Self->CommandsBuffer[0] = 0x21;
  Self->CommandsBuffer[1] = 0x00;
  Self->CommandsBuffer[2] = Self->Width - 1;
  Self->OLED_WriteCommands(Self, Self->CommandsBuffer, 3);

  Self->CommandsBuffer[0] = 0x22;
  Self->CommandsBuffer[1] = 0x00;
  Self->CommandsBuffer[2] = Self->Height / 8 - 1;
  Self->OLED_WriteCommands(Self, Self->CommandsBuffer, 3);

  Self->OLED_WriteDatas(Self, (uint8_t *) Self->DisplayBuffer, Self->Width * Self->Height / 8);
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

void OLED_Config(OLED_t *Self)
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

  uint8_t CommandsBuffer[] = {
      0xAE, // 关闭显示

      0xD5, // 设置时钟
      0x80,

      0xA8, // 设置多路复
      0x3F,

      0xD3, // 设置显示偏移
      0x00,

      0x40, // 设置起始行

      0x8D, // 设置电荷泵
      0x14,

      0xA1, // 设置左右方向 A1 A0

      0xC8, // 设置上下方向 C8 C0

      0xDA, // 设置COM硬件引脚配置
      0x12,

      0x81, // 调节亮度
      0xFF,

      0xD9, // 设置预充电周期
      0xF1,

      0xDB, // 设置VCOMH
      0x30,

      0xA4, // 全局显示开启

      0xA6, // 设置显示方式

      0x20, //
      0x00,

      0xAF // 打开显示
  };

  Self->OLED_WriteCommands(Self, CommandsBuffer, sizeof(CommandsBuffer));
}

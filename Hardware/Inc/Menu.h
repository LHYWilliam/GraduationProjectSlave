#ifndef MENU_H
#define MENU_H

#include "stm32f1xx_hal.h"

#include "OLED.h"

#define PositionUpdate(Now, Target, Step)                       \
  ((Now) += ((Now) < (Target) ? Step : (Now) > (Target) ? -Step \
                                                        : 0))

typedef enum
{
  RotationUp,
  RotationDown,
} RotationDirection;

typedef enum
{
  Int,
  Float,
} ParameterType;

typedef struct
{
  int16_t X;
  int16_t Y;
  uint8_t Width;
  uint8_t Height;

  int16_t *TargetX;
  int16_t *TargetY;
  uint8_t *TargetWidth;
  uint8_t *TargetHeight;
} SelectioneBar_t;

typedef struct TextPage
{
  char Title[32];

  int16_t X;
  int16_t Y;
  uint8_t Width;
  uint8_t Height;

  int16_t TitleX;
  int16_t TitleY;
  uint8_t TitleWidth;
  uint8_t TitleHeight;

  uint8_t Cursor;
  uint8_t NumOfLowerPages;
  struct TextPage *LowerPages;
  struct TextPage *UpperPage;

  ParameterType ParameterType;
  union
  {
    int32_t IntParameter;
    float FloatParameter;
  };
  union
  {
    int32_t *IntParameterPtr;
    float *FloatParameterPtr;
  };

  void (*ShowCallback)(struct TextPage *TextPage, OLED_t *OLED);
  void (*UpdateCallback)(struct TextPage *TextPage, OLED_t *OLED);
  void (*ClickCallback)(struct TextPage **TextPage, SelectioneBar_t *SelectioneBar);
  void (*RotationCallback)(struct TextPage *TextPage, SelectioneBar_t *SelectioneBar, RotationDirection Direction);
} TextPage_t;

void TextPage_Init(TextPage_t *TextPage, OLED_t *OLED);
ErrorStatus TextPage_CursorInc(TextPage_t *Self);
ErrorStatus TextPage_CursorDec(TextPage_t *Self);
ErrorStatus TextPage_EnterLowerPage(TextPage_t **Self);
ErrorStatus TextPage_ReturnUpperPage(TextPage_t **Self);
void TextPage_Reset(TextPage_t *Self);

void SelectioneBar_BindTextPage(SelectioneBar_t *Self, TextPage_t *TextPage);
void SelectioneBar_Update(SelectioneBar_t *Self);
void OLED_ShowSelectioneBar(OLED_t *OLED, SelectioneBar_t *SelectioneBar);

#endif

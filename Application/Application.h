#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Menu.h"

void TextPage_ShowCallback(TextPage_t *TextPage, OLED_t *OLED);

void TextPage_UpdateCallback(TextPage_t *TextPage, OLED_t *OLED);

void TextPage_BackCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);
void TextPage_EnterCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);

void TextPage_CursorCallback(TextPage_t *TextPage, SelectioneBar_t *SelectioneBar, RotationDirection Direction);

#define ShowTitleAndTexts(...)                                                          \
  if (TextPage->TitleY + TextPage->TitleHeight >= 0 && TextPage->TitleY < OLED->Height) \
  {                                                                                     \
    OLED_Printf(OLED, TextPage->TitleX, TextPage->TitleY, TextPage->Title);             \
  }                                                                                     \
                                                                                        \
  for (uint8_t i = 0; i < TextPage->NumOfLowerPages; i++)                               \
  {                                                                                     \
    if (TextPage->LowerPages[i].Y + TextPage->LowerPages[i].Height < 0)                 \
    {                                                                                   \
      continue;                                                                         \
    }                                                                                   \
    if (TextPage->LowerPages[i].Y >= OLED->Height)                                      \
    {                                                                                   \
      break;                                                                            \
    }                                                                                   \
                                                                                        \
    __VA_ARGS__                                                                         \
  }

#define TextPage_Back(title)                                     \
  (TextPage_t)                                                   \
  {                                                              \
    .Title = title, .RotationCallback = TextPage_CursorCallback, \
    .ClickCallback = TextPage_BackCallback,                      \
  }

#endif

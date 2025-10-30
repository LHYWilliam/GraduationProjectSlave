#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Menu.h"

void Application_Init(TextPage_t **TextPage, OLED_t *OLED);

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
  for (TextPage_t *Page = TextPage->HeadPage; Page != NULL; Page = Page->DownPage)      \
  {                                                                                     \
    if (Page->Y + Page->Height < 0)                                                     \
    {                                                                                   \
      continue;                                                                         \
    }                                                                                   \
    if (Page->Y >= OLED->Height)                                                        \
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

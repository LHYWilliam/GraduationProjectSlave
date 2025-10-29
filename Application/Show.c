#include "Application.h"

void TextPage_ShowCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  ShowTitleAndTexts(
      OLED_Printf(OLED, TextPage->LowerPages[i].X, TextPage->LowerPages[i].Y, TextPage->LowerPages[i].Title););
}

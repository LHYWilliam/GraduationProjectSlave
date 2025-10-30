#include "Application.h"

void TextPage_ShowCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  ShowTitleAndTexts(
      OLED_Printf(OLED, Page->X, Page->Y, Page->Title););
}

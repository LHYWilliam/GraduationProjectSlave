#include "Application.h"

void TextPage_UpdateCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  int16_t Y = TextPage->TitleY;

  if (TextPage->LowerPages->Index == 1 || TextPage->LowerPages->Index == 2)
  {
    Y = 0;
  } else if (TextPage->LowerPages->Y < 0)
  {
    Y = TextPage->TitleY - TextPage->LowerPages->Y;
  } else if (TextPage->LowerPages->Y + TextPage->LowerPages->Height >= OLED->Height)
  {
    Y = TextPage->TitleY - (TextPage->LowerPages->Y - OLED->Height + TextPage->LowerPages->Height) - 1;
  }

  PositionUpdate(TextPage->TitleY, Y, 1);

  for (TextPage_t *Page = TextPage->HeadPage; Page != NULL; Page = Page->DownPage)
  {
    if (Page->Index == 1)
    {
      Y = Y + TextPage->TitleHeight / 4 - TextPage->HeadPage->Height / 2 + 1;
    } else if (Page->Index == 2)
    {
      Y = TextPage->TitleY + TextPage->TitleHeight + 1;
    } else
    {
      Y = Page->UpPage->Y + Page->UpPage->Height + 2;
    }

    PositionUpdate(Page->Y, Y, 1);
  }
}

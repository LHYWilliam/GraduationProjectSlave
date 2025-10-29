#include "Application.h"

void TextPage_UpdateCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  int16_t Y = TextPage->TitleY;

  if (TextPage->Cursor == 0 || TextPage->Cursor == 1)
  {
    Y = 0;
  } else if (TextPage->LowerPages[TextPage->Cursor].Y < 0)
  {
    Y = TextPage->TitleY - TextPage->LowerPages[TextPage->Cursor].Y;
  } else if (TextPage->LowerPages[TextPage->Cursor].Y + TextPage->LowerPages[TextPage->Cursor].Height > OLED->Height - 1)
  {
    Y = TextPage->TitleY - (TextPage->LowerPages[TextPage->Cursor].Y - OLED->Height + TextPage->LowerPages[TextPage->Cursor].Height) - 1;
  }

  PositionUpdate(TextPage->TitleY, Y, 1);

  for (uint8_t i = 0; i < TextPage->NumOfLowerPages; i++)
  {
    if (i == 0)
    {
      Y = Y + TextPage->TitleHeight / 4 - TextPage->LowerPages[0].Height / 2 + 1;

    } else if (i == 1)
    {
      Y = TextPage->TitleY + TextPage->TitleHeight + 1;

    } else
    {
      Y = TextPage->LowerPages[i - 1].Y + TextPage->LowerPages[i - 1].Height + 2;
    }

    PositionUpdate(TextPage->LowerPages[i].Y, Y, 1);
  }
}

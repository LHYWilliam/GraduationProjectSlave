#include "Application.h"

void TextPage_UpdateCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  int16_t Y = TextPage->TitleY;

  if (TextPage->LowerPages->Index == 0 || TextPage->LowerPages->Index == 1)
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
    if (Page->Index == 0)
    {
      Y = Y + TextPage->TitleHeight / 4 - TextPage->HeadPage->Height / 2 + 1;
    } else if (Page->Index == 1)
    {
      Y = TextPage->TitleY + TextPage->TitleHeight + 1;
    } else
    {
      Y = Page->UpPage->Y + Page->UpPage->Height + 2;
    }

    PositionUpdate(Page->Y, Y, 1);
  }
}

void TextPage_UpdateDialogCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  PositionUpdate(TextPage->TitleX, OLED->Width / 8, 1);
  PositionUpdate(TextPage->TitleY, OLED->Height / 8, 1);
  PositionUpdate(TextPage->TitleWidth, OLED->Width - OLED->Width / 4, 2);
  PositionUpdate(TextPage->TitleHeight, OLED->Height - OLED->Height / 4, 2);

  PositionUpdate(TextPage->HeadPage->X, TextPage->TitleX + 4, 1);
  PositionUpdate(TextPage->HeadPage->Y, TextPage->TitleY + 4, 1);

  static uint8_t LowerPagesIndent = 8;

  uint8_t LowerPagesWidth = LowerPagesIndent;
  for (TextPage_t *Page = TextPage->HeadPage->DownPage; Page != NULL; Page = Page->DownPage)
  {
    LowerPagesWidth += Page->TitleWidth + LowerPagesIndent;
  }

  uint8_t X = OLED->Width / 2 - LowerPagesWidth / 2 + LowerPagesIndent;
  for (TextPage_t *Page = TextPage->HeadPage->DownPage; Page != NULL; Page = Page->DownPage)
  {
    PositionUpdate(Page->X, X, 1);
    PositionUpdate(Page->Y, TextPage->TitleY + TextPage->TitleHeight - OLED->FontHeight - 4, 1);

    X += Page->TitleWidth + LowerPagesIndent;
  }
}

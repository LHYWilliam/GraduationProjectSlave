#include <string.h>

#include "Menu.h"

void EmptyCallbackShowUpdate(struct TextPage *TextPage, OLED_t *OLED)
{
  UNUSED(TextPage);
  UNUSED(OLED);
}
void EmptyCallbackClick(struct TextPage **TextPage, SelectioneBar_t *SelectioneBar)
{
  UNUSED(TextPage);
  UNUSED(SelectioneBar);
}
void EmptyCallbackRotation(struct TextPage *TextPage, SelectioneBar_t *SelectioneBar, RotationDirection Direction)
{
  UNUSED(TextPage);
  UNUSED(SelectioneBar);
  UNUSED(Direction);
}

void TextPage_Init(TextPage_t *TextPage, OLED_t *OLED)
{
  if (TextPage->TitleX == 0 && TextPage->TitleY == 0 && TextPage->TitleWidth == 0 && TextPage->TitleHeight == 0)
  {
    TextPage->TitleWidth = strlen(TextPage->Title) * OLED->FontWidth;
    TextPage->TitleX = OLED->Width / 2 - TextPage->TitleWidth / 2;
    TextPage->TitleHeight = OLED->FontHeight * 2;
  }

  if (TextPage->ShowCallback == NULL)
  {
    TextPage->ShowCallback = EmptyCallbackShowUpdate;
  }
  if (TextPage->UpdateCallback == NULL)
  {
    TextPage->UpdateCallback = EmptyCallbackShowUpdate;
  }
  if (TextPage->ClickCallback == NULL)
  {
    TextPage->ClickCallback = EmptyCallbackClick;
  }
  if (TextPage->RotationCallback == NULL)
  {
    TextPage->RotationCallback = EmptyCallbackRotation;
  }

  for (TextPage_t *Page = TextPage->HeadPage; Page != NULL; Page = Page->DownPage)
  {
    Page->Width = strlen(Page->Title) * OLED->FontWidth;
    Page->Height = OLED->FontHeight;

    if (Page->UpperPage == NULL)
    {
      if (Page->Index == 0)
      {
        Page->UpperPage = TextPage->UpperPage;
      } else
      {
        Page->UpperPage = TextPage;
      }
    }

    TextPage_Init(Page, OLED);
  }
}

void TextPage_AddLowerPage(TextPage_t *Self, TextPage_t *LowerPage)
{
  if (Self->LowerPages == NULL)
  {
    Self->HeadPage = LowerPage;
    Self->TailPage = LowerPage;
    Self->LowerPages = LowerPage;
  } else
  {
    Self->TailPage->DownPage = LowerPage;
    LowerPage->UpPage = Self->TailPage;
    Self->TailPage = Self->TailPage->DownPage;
  }

  LowerPage->Index = Self->NumOfLowerPages;
  Self->NumOfLowerPages++;
}

void TextPage_CursorInc(TextPage_t *Self)
{
  if (Self->LowerPages == NULL)
  {
    return;
  }

  if (Self->LowerPages->DownPage != NULL)
  {
    Self->LowerPages = Self->LowerPages->DownPage;
  } else
  {
    Self->LowerPages = Self->HeadPage;
  }
}

void TextPage_CursorDec(TextPage_t *Self)
{
  if (Self->LowerPages == NULL)
  {
    return;
  }

  if (Self->LowerPages->UpPage != NULL)
  {
    Self->LowerPages = Self->LowerPages->UpPage;
  } else
  {
    Self->LowerPages = Self->TailPage;
  }
}

void TextPage_EnterLowerPage(TextPage_t **Self)
{
  if ((*Self)->LowerPages != NULL)
  {
    *Self = (*Self)->LowerPages;
  }
}

void TextPage_ReturnUpperPage(TextPage_t **Self)
{
  if ((*Self)->UpperPage != NULL)
  {
    *Self = (*Self)->UpperPage;
  }
}

void SelectioneBar_BindTextPage(SelectioneBar_t *SelectioneBar, TextPage_t *TextPage)
{
  SelectioneBar->TargetX = &TextPage->X;
  SelectioneBar->TargetY = &TextPage->Y;
  SelectioneBar->TargetWidth = &TextPage->Width;
  SelectioneBar->TargetHeight = &TextPage->Height;
}

void SelectioneBar_Update(SelectioneBar_t *Self)
{
  PositionUpdate(Self->X, *Self->TargetX - 1, 1);
  PositionUpdate(Self->Y, *Self->TargetY - 1, 1);
  PositionUpdate(Self->Width, *Self->TargetWidth + 2, 1);
  PositionUpdate(Self->Height, *Self->TargetHeight + 2, 1);
}

void OLED_ShowSelectioneBar(OLED_t *OLED, SelectioneBar_t *SelectioneBar)
{
  OLED_ReverseArea(OLED, SelectioneBar->X, SelectioneBar->Y, SelectioneBar->Width, SelectioneBar->Height);
}

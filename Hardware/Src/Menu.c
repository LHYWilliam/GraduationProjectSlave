#include <string.h>

#include "Menu.h"
#include "OLED.h"

void EmptyCallbackPtr(struct TextPage *TextPage)
{
  UNUSED(TextPage);
}
void EmptyCallbackPtrPtr(struct TextPage **TextPage) { UNUSED(TextPage); }
void EmptyCallbackRotation(struct TextPage *TextPage,
                           TextPageRotation Direction)
{
  UNUSED(TextPage);
  UNUSED(Direction);
}

void TextPage_Init(TextPage_t *TextPage, OLED_t *OLED)
{
  if (TextPage->TitleWidth == 0)
  {
    TextPage->TitleWidth = strlen(TextPage->Title) * OLED->FontWidth;
  }
  if (TextPage->TitleX == 0)
  {
    TextPage->TitleX = OLED->Width / 2 - TextPage->TitleWidth / 2;
  }
  if (TextPage->TitleHeight == 0)
  {
    TextPage->TitleHeight = OLED->FontHeight * 2;
  }

  for (uint8_t i = 0; i < TextPage->NumOfLowerPages; i++)
  {
    TextPage->LowerPages[i].Width = strlen(TextPage->LowerPages[i].Title) * OLED->FontWidth;
    TextPage->LowerPages[i].Height = OLED->FontHeight;

    if (i == 0)
    {
      TextPage->LowerPages[i].UpperPage = TextPage->UpperPage;
    } else
    {
      TextPage->LowerPages[i].UpperPage = TextPage;
    }

    if (TextPage->LowerPages[i].UpdateCallback == NULL)
    {
      TextPage->LowerPages[i].UpdateCallback = EmptyCallbackPtr;
    }
    if (TextPage->LowerPages[i].ShowCallback == NULL)
    {
      TextPage->LowerPages[i].ShowCallback = EmptyCallbackPtr;
    }
    if (TextPage->LowerPages[i].ClickCallback == NULL)
    {
      TextPage->LowerPages[i].ClickCallback = EmptyCallbackPtrPtr;
    }
    if (TextPage->LowerPages[i].RotationCallback == NULL)
    {
      TextPage->LowerPages[i].RotationCallback = EmptyCallbackRotation;
    }

    TextPage_Init(&TextPage->LowerPages[i], OLED);
  }
}

void TextPage_Reset(TextPage_t *Self)
{
  for (uint8_t i = Self->Cursor; i < Self->NumOfLowerPages; i++)
  {
    Self->LowerPages[i].Y = 0;
  }
  for (int8_t i = Self->Cursor - 1; i >= 0; i--)
  {
    Self->LowerPages[i].Y = Self->LowerPages[i + 1].Y - Self->LowerPages[i].Height - 2;
  }
}

ErrorStatus TextPage_CursorInc(TextPage_t *Self)
{
  if (Self->NumOfLowerPages >= 2)
  {
    Self->Cursor = (Self->Cursor + 1) % Self->NumOfLowerPages;

    return SUCCESS;
  }

  return ERROR;
}

ErrorStatus TextPage_CursorDec(TextPage_t *Self)
{
  if (Self->NumOfLowerPages >= 2)
  {
    Self->Cursor = (Self->Cursor + Self->NumOfLowerPages - 1) % Self->NumOfLowerPages;

    return SUCCESS;
  }

  return ERROR;
}

ErrorStatus TextPage_EnterLowerPage(TextPage_t **Self)
{
  if ((*Self)->LowerPages[(*Self)->Cursor].NumOfLowerPages != 0)
  {
    *Self = &(*Self)->LowerPages[(*Self)->Cursor];

    return SUCCESS;
  }

  return ERROR;
}

ErrorStatus TextPage_ReturnUpperPage(TextPage_t **Self)
{
  if ((*Self)->UpperPage != NULL)
  {
    *Self = (*Self)->UpperPage;

    return SUCCESS;
  }

  return ERROR;
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
  OLED_ReverseArea(OLED, SelectioneBar->X, SelectioneBar->Y,
                   SelectioneBar->Width, SelectioneBar->Height);
}

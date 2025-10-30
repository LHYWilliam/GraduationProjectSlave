#include "Application.h"

void TextPage_CursorCallback(TextPage_t *TextPage, SelectioneBar_t *SelectioneBar, RotationDirection Direction)
{
  switch (Direction)
  {
  case RotationUp:
    TextPage_CursorDec(TextPage);
    SelectioneBar_BindTextPage(SelectioneBar, TextPage->LowerPages);

    break;

  case RotationDown:
    TextPage_CursorInc(TextPage);
    SelectioneBar_BindTextPage(SelectioneBar, TextPage->LowerPages);

    break;
  }
}

#include "Application.h"

void TextPage_CursorCallback(TextPage_t *TextPage, SelectioneBar_t *SelectioneBar, RotationDirection Direction)
{
  switch (Direction)
  {
  case RotationUp:
    if (TextPage_CursorDec(TextPage) == SUCCESS)
    {
      SelectioneBar_BindTextPage(SelectioneBar, &TextPage->LowerPages[TextPage->Cursor]);
    }
    break;

  case RotationDown:
    if (TextPage_CursorInc(TextPage) == SUCCESS)
    {
      SelectioneBar_BindTextPage(SelectioneBar, &TextPage->LowerPages[TextPage->Cursor]);
    }
    break;
  }
}

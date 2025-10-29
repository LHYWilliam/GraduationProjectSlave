#include "Application.h"

void TextPage_BackCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar)
{
  if (TextPage_ReturnUpperPage(TextPage) == SUCCESS)
  {
    SelectioneBar_BindTextPage(SelectioneBar, &(*TextPage)->LowerPages[(*TextPage)->Cursor]);
  }
}

void TextPage_EnterCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar)
{
  if (TextPage_EnterLowerPage(TextPage) == SUCCESS)
  {
    SelectioneBar_BindTextPage(SelectioneBar, &(*TextPage)->LowerPages[(*TextPage)->Cursor]);
  }
}

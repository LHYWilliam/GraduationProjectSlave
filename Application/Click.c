#include "Application.h"

void TextPage_BackCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar)
{
  TextPage_ReturnUpperPage(TextPage);
  SelectioneBar_BindTextPage(SelectioneBar, (*TextPage)->LowerPages);
}

void TextPage_EnterCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar)
{
  TextPage_EnterLowerPage(TextPage);
  SelectioneBar_BindTextPage(SelectioneBar, (*TextPage)->LowerPages);
}

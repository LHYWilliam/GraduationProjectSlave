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

void TextPage_ChooseOptionCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar)
{
  UNUSED(SelectioneBar);

  TextPage_t *Page = *TextPage;

  if (*(Page->IntParameterPtr) == 0)
  {
    *(Page->IntParameterPtr) = Page->LowerPages->Index;
  } else if (*(Page->IntParameterPtr) == Page->LowerPages->Index)
  {
    *(Page->IntParameterPtr) = 0;
  } else
  {
    *(Page->IntParameterPtr) = Page->LowerPages->Index;
  }
}

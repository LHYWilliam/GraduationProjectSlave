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

void TextPage_LoRaSettingApplyCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar)
{
  LoRa_ATMode(&LoRa);
  LoRa_LoadConfigFromArray(&LoRa, LoRa.ConfigArray);
  LoRa_CommunicationMode(&LoRa);

  TextPage_EnterCallback(TextPage, SelectioneBar);
}

void TextPage_LoRaSettingReadCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar)
{
  LoRa_ATMode(&LoRa);
  LoRa_ReadConfig(&LoRa);
  LoRa_CommunicationMode(&LoRa);

  TextPage_EnterCallback(TextPage, SelectioneBar);
}

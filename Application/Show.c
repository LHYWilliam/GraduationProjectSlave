#include "Application.h"

void TextPage_ShowCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  ShowTitleAndTexts(
      OLED_Printf(OLED, Page->X, Page->Y, Page->Title););
}

void TextPage_ShowMQSensorCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  TextPage_t *Page = TextPage->HeadPage;

  OLED_Printf(OLED, Page->X, Page->Y, Page->Title);

  // OLED_Printf(&OLED, 16, 1, "%s %s", TextMenu.Page->Title, MQSensor[TextMenu.Page->UpperPage->Cursor - 1].State ? "Danger" : "Safe");

  // OLED_Printf(OLED, 0, OLED->Height - OLED->FontHeight, "%.3f V", ADCToVoltage(MQSensor[TextMenu.Page->UpperPage->Cursor - 1].Data[MQSensor[TextMenu.Page->UpperPage->Cursor - 1].Index]));

  Page = Page->DownPage;

  OLED_DrawChart(OLED,
                 Page->TitleX, Page->TitleY,
                 Page->TitleWidth, Page->TitleHeight,
                 MQxSensor[Page->UpperPage->Index - 1].Data,
                 MQxSensor[Page->UpperPage->Index - 1].Length,
                 MQxSensor[Page->UpperPage->Index - 1].Index);
  // OLED_DrawHLine(OLED, TextMenu.Page->TitleX, OLED_ADCToY(MQSensor[TextMenu.Page->UpperPage->Cursor - 1].Threshold, TextMenu.Page->TitleY, TextMenu.Page->TitleHeight), TextMenu.Page->TitleWidth, 2);
}

#include "Application.h"

void TextPage_ShowCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  ShowTitleAndTexts(
      OLED_Printf(OLED, Page->X, Page->Y, Page->Title););
}

void TextPage_ShowMQxPageCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  ShowTitleAndTexts(
      if (Page->Index == 0) {
        OLED_Printf(OLED, Page->X, Page->Y, Page->Title);
      } else {
        OLED_Printf(OLED, Page->X, Page->Y, Page->Title);
        OLED_Printf(OLED, OLED->Width / 2, Page->Y, "%.3f V",
                    ADCToVoltage(MQSensor_GetData(&MQxSensor[Page->Index - 1])));
      });
}

void TextPage_ShowMQPageCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  uint8_t Index = TextPage->Index - 1;
  TextPage_t *Page = TextPage->HeadPage;

  OLED_Printf(OLED, Page->X, Page->Y, Page->Title);
  OLED_Printf(
      OLED,
      Page->X + OLED->FontWidth * 2,
      Page->Y,
      "%s %s",
      TextPage->Title, MQxSensor[Index].State ? "Safe" : "Danger");
  OLED_Printf(
      OLED,
      Page->X,
      OLED->Height - OLED->FontHeight,
      "%.3f V",
      ADCToVoltage(MQSensor_GetData(&MQxSensor[Index])));

  Page = Page->DownPage;

  OLED_DrawChart(
      OLED,
      Page->TitleX, Page->TitleY,
      Page->TitleWidth, Page->TitleHeight,
      MQxSensor[Index].Data,
      MQxSensor[Index].Length,
      MQxSensor[Index].Index);

  OLED_DrawHLine(
      OLED,
      Page->TitleX,
      Normalization(4095 - MQxSensor[Index].Threshold, 4095, Page->TitleHeight - 1, Page->TitleY),
      Page->TitleWidth,
      2);
}

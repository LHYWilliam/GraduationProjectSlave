#include "Application.h"

void TextPage_ShowCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  ShowTitleAndTexts(OLED_Printf(OLED, Page->X, Page->Y, Page->Title););
}

void TextPage_ShowMQxPageCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  ShowTitleAndTexts(
      if (Page->Index == 0) { OLED_Printf(OLED, Page->X, Page->Y, Page->Title); } else {
        OLED_Printf(OLED, Page->X, Page->Y, Page->Title);

        uint16_t ADCValue = MQSensor_GetData(&MQxSensor[Page->Index - 1]);
        uint16_t PPM;

        if (Page->Index == 1)
        {
          PPM = MQSensor_CalculateMQ2PPM(ADCValue);
        } else if (Page->Index == 2)
        {
          PPM = MQSensor_CalculateMQ3PPM(ADCValue);
        }

        OLED_Printf(OLED, OLED->Width / 2, Page->Y, "%d PPM", PPM);
      });
}

void TextPage_ShowMQPageCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  uint8_t Index = TextPage->Index - 1;
  TextPage_t *Page = TextPage->HeadPage;

  OLED_Printf(OLED, Page->X, Page->Y, Page->Title);
  OLED_Printf(OLED, Page->X + OLED->FontWidth * 2, Page->Y, "%s %s", TextPage->Title,
              MQxSensor[Index].State ? "Safe" : "Danger");

  uint16_t ADCValue = MQSensor_GetData(&MQxSensor[Index]);
  uint16_t PPM;

  if (TextPage->Index == 1)
  {
    PPM = MQSensor_CalculateMQ2PPM(ADCValue);
  } else if (TextPage->Index == 2)
  {
    PPM = MQSensor_CalculateMQ3PPM(ADCValue);
  }

  OLED_Printf(OLED, Page->X, OLED->Height - OLED->FontHeight, "%d PPM", PPM);

  Page = Page->DownPage;

  OLED_DrawChart(OLED, Page->TitleX, Page->TitleY, Page->TitleWidth, Page->TitleHeight,
                 MQxSensor[Index].Data, MQxSensor[Index].Length, MQxSensor[Index].Index);

  OLED_DrawHLine(
      OLED, Page->TitleX,
      Normalization(4095 - MQxSensor[Index].Threshold, 4095, Page->TitleHeight - 1, Page->TitleY),
      Page->TitleWidth, 2);
}

void TextPage_ShowOptionPageCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  ShowTitleAndTexts(OLED_Printf(OLED, Page->X, Page->Y, Page->Title);

                    if (Page->Index > 0) {
                      OLED_DrawHollowRectangle(OLED, OLED->Width - 16, Page->Y, 8, 8, 1);
                      if (*TextPage->IntParameterPtr == Page->Index)
                      {
                        OLED_DrawSolidRectangle(OLED, OLED->Width - 16 + 2, Page->Y + 2, 4, 4);
                      }
                    });
}

void TextPage_ShowDialogCallback(TextPage_t *TextPage, OLED_t *OLED)
{
  TextPage->UpperPage->ShowCallback(TextPage->UpperPage, OLED);

  OLED_ClearBufferArea(OLED, TextPage->TitleX, TextPage->TitleY, TextPage->TitleWidth,
                       TextPage->TitleHeight);
  OLED_DrawHollowRectangle(OLED, TextPage->TitleX, TextPage->TitleY, TextPage->TitleWidth,
                           TextPage->TitleHeight, 1);

  for (TextPage_t *Page = TextPage->HeadPage; Page != NULL; Page = Page->DownPage)
  {
    if (Page->X < TextPage->TitleX || Page->Y < TextPage->TitleY ||
        Page->X + Page->Width > TextPage->TitleX + TextPage->TitleWidth ||
        Page->Y + Page->Height > TextPage->TitleY + TextPage->TitleHeight)
    {
      continue;
    }

    OLED_Printf(OLED, Page->X, Page->Y, Page->Title);
    OLED_DrawHollowRectangle(OLED, Page->X - 2, Page->Y - 2, Page->Width + 4, Page->Height + 4, 1);
  }
}

void TextPage_ShowLoRaPage(TextPage_t *TextPage, OLED_t *OLED)
{
  ShowTitleAndTexts(
      OLED_Printf(OLED, Page->X, Page->Y, Page->Title);
    
    switch (Page->Index) {
    case 1:
      OLED_Printf(OLED, OLED->Width - OLED->FontWidth * 6, Page->Y, "0x%02X",
                  *Page->IntParameterPtr);
      break;
    case 2:
    case 3:
      OLED_ShowImage(OLED, OLED->Width - OLED->FontWidth * 6, Page->Y, 8, 8,
                     *Page->IntParameterPtr ? SettingImage[1] : SettingImage[0]);
      break;
    });
}

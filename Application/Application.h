#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "Key.h"
#include "LED.h"
#include "OLED.h"
#include "Menu.h"
#include "Serial.h"
#include "Sampler.h"
#include "Encoder.h"
#include "MQSensor.h"

extern LED_t BoardLED;
extern Key_t BoardKey;
extern Key_t EncoderKey;
extern OLED_t OLED;
extern Serial_t Serial;
extern Sampler_t Sampler;
extern Encoder_t Encoder;
extern MQSensor_t MQxSensor[2];

extern TextPage_t *TextPage;
extern SelectioneBar_t SelectioneBar;

void Application_Init(void);

void TextPage_ShowCallback(TextPage_t *TextPage, OLED_t *OLED);
void TextPage_ShowMQxPageCallback(TextPage_t *TextPage, OLED_t *OLED);
void TextPage_ShowMQPageCallback(TextPage_t *TextPage, OLED_t *OLED);
void TextPage_UpdateCallback(TextPage_t *TextPage, OLED_t *OLED);
void TextPage_BackCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);
void TextPage_EnterCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);
void TextPage_CursorCallback(TextPage_t *TextPage, SelectioneBar_t *SelectioneBar, RotationDirection Direction);

#define ShowTitleAndTexts(...)                                                          \
  if (TextPage->TitleY + TextPage->TitleHeight >= 0 && TextPage->TitleY < OLED->Height) \
  {                                                                                     \
    OLED_Printf(OLED, TextPage->TitleX, TextPage->TitleY, TextPage->Title);             \
  }                                                                                     \
                                                                                        \
  for (TextPage_t *Page = TextPage->HeadPage; Page != NULL; Page = Page->DownPage)      \
  {                                                                                     \
    if (Page->Y + Page->Height < 0)                                                     \
    {                                                                                   \
      continue;                                                                         \
    }                                                                                   \
    if (Page->Y >= OLED->Height)                                                        \
    {                                                                                   \
      break;                                                                            \
    }                                                                                   \
                                                                                        \
    __VA_ARGS__                                                                         \
  }

#define TextPage_BackPage(title)                   \
  {                                                \
      .Title = title,                              \
      .ClickCallback = TextPage_BackCallback,      \
      .RotationCallback = TextPage_CursorCallback, \
  }

#define TextPage_NavigationPage(title)             \
  {                                                \
      .Title = title,                              \
      .ShowCallback = TextPage_ShowCallback,       \
      .UpdateCallback = TextPage_UpdateCallback,   \
      .ClickCallback = TextPage_EnterCallback,     \
      .RotationCallback = TextPage_CursorCallback, \
  }

#define TextPage_MQxPage(title)                    \
  {                                                \
      .Title = title,                              \
      .ShowCallback = TextPage_ShowMQPageCallback, \
      .ClickCallback = TextPage_EnterCallback,     \
      .RotationCallback = TextPage_CursorCallback, \
  }

#define TextPage_ChartPage(title)     \
  {                                   \
      .Title = title,                 \
      .TitleY = OLED.Height / 4,      \
      .TitleWidth = OLED.Width,       \
      .TitleHeight = OLED.Height / 2, \
  }

#define ADCToVoltage(ADC) ((float) (ADC) * 3.3 / 4095.0)
#define VoltageToADC(Voltage) ((uint16_t) ((Voltage) / 3.3 * 4095.0))

#endif

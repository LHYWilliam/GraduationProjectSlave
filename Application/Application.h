#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <string.h>

#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "LED.h"
#include "Key.h"

#include "OLED.h"
#include "Menu.h"
#include "Encoder.h"

#include "Sampler.h"
#include "MQSensor.h"

#include "LoRa.h"
#include "Serial.h"

#define Uint8sToUint32(Data1, Data2, Data3, Data4)                                                 \
  (((uint32_t) (Data1) << 24) | ((uint32_t) (Data2) << 16) | ((uint32_t) (Data3) << 8) |           \
   ((uint32_t) (Data4) << 0))

#define Uint8ArrayToUint32(Datas) (Uint8sToUint32((Datas)[0], (Datas)[1], (Datas)[2], (Datas)[3]))

#define Uint16ToUint8s(Data) ((Data) >> 8) & 0xFF, ((Data) >> 0) & 0xFF

#define Uint32ToUint8s(Data)                                                                       \
  ((Data) >> 24) & 0xFF, ((Data) >> 16) & 0xFF, ((Data) >> 8) & 0xFF, ((Data) >> 0) & 0xFF

typedef enum
{
  MasterBroadcast,
  SlaveRegister,
  SlaveRegisterAck,
  SlaveHeartbeat,
  SlaveUpload,
  SlaveLogout,
} MessageType_t;

typedef struct
{
  uint8_t Head;
  MessageType_t Type;
  uint8_t DeviceID;
  uint8_t Length;
  uint8_t Data[64];
  uint32_t Tick;
} Message_t;

typedef struct
{
  int32_t ID;
  // int32_t Online;
  int32_t Upload;
  int32_t Register;

  uint32_t UID;
  uint8_t Connecting;
  uint32_t LastRegisterRetryTick;
  uint32_t LastHeartBeatTick;
  uint32_t LastUploadTick;
} Controller_t;

extern LED_t BoardLED;
extern Key_t BoardKey;

extern OLED_t OLED;
extern Key_t EncoderKey;
extern Encoder_t Encoder;
extern TextPage_t *TextPage;
extern SelectioneBar_t SelectioneBar;

extern Sampler_t Sampler;
extern MQSensor_t MQxSensor[2];

extern Serial_t Serial;
extern LoRa_t LoRa;

#define PageCount 4
extern uint8_t TextPageIndex;
extern uint8_t TextPageCount;
extern TextPage_t *TextPages[PageCount];

extern Controller_t Controller;

void Application_Init(void);

void TextPage_ShowCallback(TextPage_t *TextPage, OLED_t *OLED);
void TextPage_ShowMQxPageCallback(TextPage_t *TextPage, OLED_t *OLED);
void TextPage_ShowMQPageCallback(TextPage_t *TextPage, OLED_t *OLED);
void TextPage_ShowOptionPageCallback(TextPage_t *TextPage, OLED_t *OLED);
void TextPage_ShowDialogCallback(TextPage_t *TextPage, OLED_t *OLED);
void TextPage_ShowLoRaPage(TextPage_t *TextPage, OLED_t *OLED);

void TextPage_UpdateCallback(TextPage_t *TextPage, OLED_t *OLED);
void TextPage_UpdateDialogCallback(TextPage_t *TextPage, OLED_t *OLED);

void TextPage_BackCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);
void TextPage_EnterCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);
void TextPage_ChooseOptionCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);
void TextPage_LoRaSettingApplyCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);
void TextPage_LoRaSettingReadCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);
void TextPage_NextPageCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);
void TextPage_UploadCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);
void TextPage_RegisterCallback(TextPage_t **TextPage, SelectioneBar_t *SelectioneBar);

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

#define TextPage_EmptyPage(title) \
  {                               \
      .Title = title,             \
  }

#define TextPage_NextPage(title)                  \
  {                                               \
      .Title = title,                             \
      .ClickCallback = TextPage_NextPageCallback, \
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

#define TextPage_OptionGroupPage(title, Ptr)       \
  {                                                \
      .Title = title,                              \
      .ShowCallback = TextPage_ShowCallback,       \
      .UpdateCallback = TextPage_UpdateCallback,   \
      .ClickCallback = TextPage_EnterCallback,     \
      .RotationCallback = TextPage_CursorCallback, \
      .IntParameterPtr = Ptr,                      \
  }

#define TextPage_OptionPage(title)                    \
  {                                                   \
      .Title = title,                                 \
      .ClickCallback = TextPage_ChooseOptionCallback, \
      .RotationCallback = TextPage_CursorCallback,    \
  }

#define TextPage_AddBackPage(Page)                       \
  do                                                     \
  {                                                      \
    static TextPage_t BackPage = TextPage_BackPage("<"); \
    TextPage_AddLowerPage(Page, &BackPage);              \
  } while (0)

#define TextPage_AddOptionGroup(UpperPage, OptionGroup, Title, Ptr)     \
  static TextPage_t OptionGroup = TextPage_OptionGroupPage(Title, Ptr); \
  OptionGroup.ShowCallback = TextPage_ShowOptionPageCallback;           \
  TextPage_AddLowerPage(UpperPage, &OptionGroup);


#define TextPage_AddOption(UpperPage, Option)                   \
  do                                                            \
  {                                                             \
    static TextPage_t OptionPage = TextPage_OptionPage(Option); \
    TextPage_AddLowerPage(UpperPage, &OptionPage);              \
  } while (0)

#define TextPage_DialogPage(title)                     \
  {                                                    \
      .Title = title,                                  \
      .TitleX = OLED.Width / 2,                        \
      .TitleY = OLED.Height / 2,                       \
      .ShowCallback = TextPage_ShowDialogCallback,     \
      .UpdateCallback = TextPage_UpdateDialogCallback, \
      .ClickCallback = TextPage_EnterCallback,         \
      .RotationCallback = TextPage_CursorCallback,     \
  }

#define ADCToVoltage(ADC) ((float) (ADC) * 3.3 / 4095.0)
#define VoltageToADC(Voltage) ((uint16_t) ((Voltage) / 3.3 * 4095.0))

#endif

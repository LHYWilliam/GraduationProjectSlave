#include "main.h"

#include "Application.h"

LED_t BoardLED = {
    .GPIOx = BoardLED_GPIO_Port,
    .Pin = BoardLED_Pin,
    .ActiveState = GPIO_PIN_SET,
};

Key_t BoardKey = {
    .GPIOx = BoardKey_GPIO_Port,
    .Pin = BoardKey_Pin,
    .ActiveState = GPIO_PIN_SET,
};

Key_t EncoderKey = {
    .GPIOx = EncoderKey_GPIO_Port,
    .Pin = EncoderKey_Pin,
    .ActiveState = GPIO_PIN_RESET,
};

OLED_t OLED = {
    .hI2Cx = &hi2c1,
};

Serial_t Serial = {
    .hUARTx = &huart3,
};

uint16_t Buffer[2];
Sampler_t Sampler = {
    .hADCx = &hadc1,
    .hTIMx = &htim3,
    .Length = 2,
    .Buffer = Buffer,
};

Encoder_t Encoder = {
    .hTIMx = &htim1,
};

TextPage_t *TextPage;
SelectioneBar_t SelectioneBar;

#define MQ2DataLength 128
uint16_t MQ2Data[MQ2DataLength];
MQSensor_t MQ2Sensor = {
    .Data = MQ2Data,
    .Length = MQ2DataLength,
};

#define MQ3DataLength 128
uint16_t MQ3Data[MQ3DataLength];
MQSensor_t MQ3Sensor = {
    .Data = MQ3Data,
    .Length = MQ3DataLength,
};

MQSensor_t MQxSensor[2] = {
    {
        .Data = MQ2Data,
        .Length = MQ2DataLength,
    },
    {
        .Data = MQ3Data,
        .Length = MQ3DataLength,
    },
};


void Application_Init(TextPage_t **TextPage, OLED_t *OLED)
{
  static TextPage_t HomePage =
      {
          .Title = "Home",
          .ShowCallback = TextPage_ShowCallback,
          .UpdateCallback = TextPage_UpdateCallback,
      };
  *TextPage = &HomePage;

  static TextPage_t PageBack = TextPage_Back("<");
  TextPage_AddLowerPage(&HomePage, &PageBack);

  static TextPage_t MQxPage =
      {
          .Title = "MQxPage",
          .ShowCallback = TextPage_ShowCallback,
          .UpdateCallback = TextPage_UpdateCallback,
          .ClickCallback = TextPage_EnterCallback,
          .RotationCallback = TextPage_CursorCallback,
      };
  TextPage_AddLowerPage(&HomePage, &MQxPage);

  {
    static TextPage_t PageBack = TextPage_Back("<");
    TextPage_AddLowerPage(&MQxPage, &PageBack);

    {
      static TextPage_t MQ2Page;
      MQ2Page = (TextPage_t) {
          .Title = "MQ2",
          .ShowCallback = TextPage_ShowMQSensorCallback,
          .ClickCallback = TextPage_EnterCallback,
          .RotationCallback = TextPage_CursorCallback,
      };
      TextPage_AddLowerPage(&MQxPage, &MQ2Page);

      {
        static TextPage_t PageBack = {
            .Title = "<",
            .ClickCallback = TextPage_BackCallback,
        };
        TextPage_AddLowerPage(&MQ2Page, &PageBack);

        static TextPage_t MQ2ChartPage;
        MQ2ChartPage = (TextPage_t) TextPage_Chart("MQ2");
        TextPage_AddLowerPage(&MQ2Page, &MQ2ChartPage);
      }

      static TextPage_t MQ3Page;
      MQ3Page = (TextPage_t) {
          .Title = "MQ3",
          .ShowCallback = TextPage_ShowMQSensorCallback,
          .ClickCallback = TextPage_EnterCallback,
          .RotationCallback = TextPage_CursorCallback,
      };
      TextPage_AddLowerPage(&MQxPage, &MQ3Page);

      {
        static TextPage_t PageBack = {
            .Title = "<",
            .ClickCallback = TextPage_BackCallback,
        };
        TextPage_AddLowerPage(&MQ3Page, &PageBack);

        static TextPage_t MQ3ChartPage;
        MQ3ChartPage = (TextPage_t) TextPage_Chart("MQ3");
        TextPage_AddLowerPage(&MQ3Page, &MQ3ChartPage);
      }
    }
  }

  TextPage_Init(*TextPage, OLED);
}

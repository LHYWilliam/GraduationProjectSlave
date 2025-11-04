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

OLED_t OLED = {
    .hI2Cx = &hi2c1,
};

Key_t EncoderKey = {
    .GPIOx = EncoderKey_GPIO_Port,
    .Pin = EncoderKey_Pin,
    .ActiveState = GPIO_PIN_RESET,
};

Encoder_t Encoder = {
    .hTIMx = &htim1,
};

TextPage_t *TextPage;
SelectioneBar_t SelectioneBar;

uint16_t Buffer[2];
Sampler_t Sampler = {
    .hADCx = &hadc1,
    .hTIMx = &htim3,
    .Length = 2,
    .Buffer = Buffer,
};

#define MQ2DataLength 128
uint16_t MQ2Data[MQ2DataLength];

#define MQ3DataLength 128
uint16_t MQ3Data[MQ3DataLength];

#define Threshold Threshold = VoltageToADC(2.048), .Relaxation = VoltageToADC(0.128)

MQSensor_t MQxSensor[2] = {
    {
        .Data = MQ2Data,
        .Length = MQ2DataLength,
        .Threshold,
    },
    {
        .Data = MQ3Data,
        .Length = MQ3DataLength,
        .Threshold,
    },
};

Serial_t Serial = {
    .hUARTx = &huart3,
};

LoRa_t LoRa = {
    .hUARTx = &huart2,
    .AUX_Port = LoRaAUX_GPIO_Port,
    .AUX_Pin = LoRaAUX_Pin,
    .MD0_Port = LoRaMD0_GPIO_Port,
    .MD0_Pin = LoRaMD0_Pin,
    // .Config = {
    //     .BaudRate = LoRaBaudRate115200,
    //     .Parity = LoRaParityNone,
    //     .Address = 0x0000,
    //     .Channel = 23,
    //     .WLRate = LoRaWLRate19_2Kbps,
    //     .TPower = LoRaTPower20dBm,
    //     .WLTime = LoRaWLTime1s,
    //     .TMode = LoRaTModeTransparentTransmission,
    //     .CWMode = LoRaCWModeNormal,
    // },
};

void Application_Init(void)
{
  OLED_Init(&OLED);
  Encoder_Start(&Encoder);

  Sampler_Start_DMA_TIM_IT(&Sampler);

  LoRa_StartIdleIT(&LoRa);
  LoRa_ATMode(&LoRa);
  // LoRa_ApplyConfig(&LoRa, &LoRa.Config);
  LoRa_ReadConfig(&LoRa);
  LoRa_CommunicationMode(&LoRa);

  static TextPage_t HomePage = TextPage_NavigationPage("Home");
  TextPage = &HomePage;

  static TextPage_t BackPage = TextPage_BackPage("<");
  TextPage_AddLowerPage(&HomePage, &BackPage);

  static TextPage_t MQxSensorPage = TextPage_NavigationPage("MQxSensor");
  MQxSensorPage.ShowCallback = TextPage_ShowMQxPageCallback;
  TextPage_AddLowerPage(&HomePage, &MQxSensorPage);

  {
    static TextPage_t BackPage = TextPage_BackPage("<");
    TextPage_AddLowerPage(&MQxSensorPage, &BackPage);

    {
      static TextPage_t MQ2Page = TextPage_MQxPage("MQ2");
      TextPage_AddLowerPage(&MQxSensorPage, &MQ2Page);

      {
        static TextPage_t BackPage = TextPage_BackPage("<");
        BackPage.Y = 1;
        BackPage.RotationCallback = NULL;
        TextPage_AddLowerPage(&MQ2Page, &BackPage);

        static TextPage_t MQ2ChartPage;
        MQ2ChartPage = (TextPage_t) TextPage_ChartPage("MQ2");
        TextPage_AddLowerPage(&MQ2Page, &MQ2ChartPage);
      }

      static TextPage_t MQ3Page = TextPage_MQxPage("MQ3");
      TextPage_AddLowerPage(&MQxSensorPage, &MQ3Page);

      {
        static TextPage_t BackPage = TextPage_BackPage("<");
        BackPage.Y = 1;
        BackPage.RotationCallback = NULL;
        TextPage_AddLowerPage(&MQ3Page, &BackPage);

        static TextPage_t MQ3ChartPage;
        MQ3ChartPage = (TextPage_t) TextPage_ChartPage("MQ3");
        TextPage_AddLowerPage(&MQ3Page, &MQ3ChartPage);
      }
    }
  }

  static TextPage_t LoRaSettingPage = TextPage_NavigationPage("Setting");
  TextPage_AddLowerPage(&HomePage, &LoRaSettingPage);

  {
    static TextPage_t BackPage = TextPage_BackPage("<");
    TextPage_AddLowerPage(&LoRaSettingPage, &BackPage);

    static TextPage_t LoRaBaudRatePage = TextPage_OptionGroupPage("BaudRate", IntParameterPtr, &LoRa.ConfigArray[0]);
    LoRaBaudRatePage.ShowCallback = TextPage_ShowOptionPageCallback;
    TextPage_AddLowerPage(&LoRaSettingPage, &LoRaBaudRatePage);

    {
      static TextPage_t BackPage = TextPage_BackPage("<");
      TextPage_AddLowerPage(&LoRaBaudRatePage, &BackPage);

      TextPage_AddOption(&LoRaBaudRatePage, "1200");
      TextPage_AddOption(&LoRaBaudRatePage, "2400");
      TextPage_AddOption(&LoRaBaudRatePage, "4800");
      TextPage_AddOption(&LoRaBaudRatePage, "9600");
      TextPage_AddOption(&LoRaBaudRatePage, "19200");
      TextPage_AddOption(&LoRaBaudRatePage, "38400");
      TextPage_AddOption(&LoRaBaudRatePage, "57600");
      TextPage_AddOption(&LoRaBaudRatePage, "115200");
    }

    static TextPage_t LoRaParityPage = TextPage_OptionGroupPage("Parity", IntParameterPtr, &LoRa.ConfigArray[1]);
    LoRaParityPage.ShowCallback = TextPage_ShowOptionPageCallback;
    TextPage_AddLowerPage(&LoRaSettingPage, &LoRaParityPage);

    {
      static TextPage_t BackPage = TextPage_BackPage("<");
      TextPage_AddLowerPage(&LoRaParityPage, &BackPage);

      TextPage_AddOption(&LoRaParityPage, "None");
      TextPage_AddOption(&LoRaParityPage, "Even");
      TextPage_AddOption(&LoRaParityPage, "Odd");
    }

    static TextPage_t LoRaAddressPage = TextPage_NavigationPage("Address");
    LoRaAddressPage.ShowCallback = TextPage_ShowOptionPageCallback;
    TextPage_AddLowerPage(&LoRaSettingPage, &LoRaAddressPage);

    {
      static TextPage_t BackPage = TextPage_BackPage("<");
      TextPage_AddLowerPage(&LoRaAddressPage, &BackPage);
    }

    static TextPage_t LoRaChannelPage = TextPage_NavigationPage("Channel");
    LoRaChannelPage.ShowCallback = TextPage_ShowOptionPageCallback;
    TextPage_AddLowerPage(&LoRaSettingPage, &LoRaChannelPage);

    {
      static TextPage_t BackPage = TextPage_BackPage("<");
      TextPage_AddLowerPage(&LoRaChannelPage, &BackPage);
    }

    static TextPage_t LoRaWLRatePage = TextPage_OptionGroupPage("WLRate", IntParameterPtr, &LoRa.ConfigArray[4]);
    LoRaWLRatePage.ShowCallback = TextPage_ShowOptionPageCallback;
    TextPage_AddLowerPage(&LoRaSettingPage, &LoRaWLRatePage);

    {
      static TextPage_t BackPage = TextPage_BackPage("<");
      TextPage_AddLowerPage(&LoRaWLRatePage, &BackPage);

      TextPage_AddOption(&LoRaWLRatePage, "0.3Kbps");
      TextPage_AddOption(&LoRaWLRatePage, "1.2Kbps");
      TextPage_AddOption(&LoRaWLRatePage, "2.4Kbps");
      TextPage_AddOption(&LoRaWLRatePage, "4.8Kbps");
      TextPage_AddOption(&LoRaWLRatePage, "9.6Kbps");
      TextPage_AddOption(&LoRaWLRatePage, "19.2Kbps");
    }

    static TextPage_t LoRaTPowerPage = TextPage_OptionGroupPage("TPower", IntParameterPtr, &LoRa.ConfigArray[5]);
    LoRaTPowerPage.ShowCallback = TextPage_ShowOptionPageCallback;
    TextPage_AddLowerPage(&LoRaSettingPage, &LoRaTPowerPage);

    {
      static TextPage_t BackPage = TextPage_BackPage("<");
      TextPage_AddLowerPage(&LoRaTPowerPage, &BackPage);

      TextPage_AddOption(&LoRaTPowerPage, "11dBm");
      TextPage_AddOption(&LoRaTPowerPage, "14dBm");
      TextPage_AddOption(&LoRaTPowerPage, "17dBm");
      TextPage_AddOption(&LoRaTPowerPage, "20dBm");
    }

    static TextPage_t LoRaWLTimePage = TextPage_OptionGroupPage("WLTime", IntParameterPtr, &LoRa.ConfigArray[6]);
    LoRaWLTimePage.ShowCallback = TextPage_ShowOptionPageCallback;
    TextPage_AddLowerPage(&LoRaSettingPage, &LoRaWLTimePage);

    {
      static TextPage_t BackPage = TextPage_BackPage("<");
      TextPage_AddLowerPage(&LoRaWLTimePage, &BackPage);

      TextPage_AddOption(&LoRaWLTimePage, "1s");
      TextPage_AddOption(&LoRaWLTimePage, "2s");
    }

    static TextPage_t LoRaTModePage = TextPage_OptionGroupPage("TMode", IntParameterPtr, &LoRa.ConfigArray[7]);
    LoRaTModePage.ShowCallback = TextPage_ShowOptionPageCallback;
    TextPage_AddLowerPage(&LoRaSettingPage, &LoRaTModePage);

    {
      static TextPage_t BackPage = TextPage_BackPage("<");
      TextPage_AddLowerPage(&LoRaTModePage, &BackPage);

      TextPage_AddOption(&LoRaTModePage, "Transparent");
      TextPage_AddOption(&LoRaTModePage, "Directed");
    }

    static TextPage_t LoRaCWModePage = TextPage_OptionGroupPage("CWMode", IntParameterPtr, &LoRa.ConfigArray[8]);
    LoRaCWModePage.ShowCallback = TextPage_ShowOptionPageCallback;
    TextPage_AddLowerPage(&LoRaSettingPage, &LoRaCWModePage);

    {
      static TextPage_t BackPage = TextPage_BackPage("<");
      TextPage_AddLowerPage(&LoRaCWModePage, &BackPage);

      TextPage_AddOption(&LoRaCWModePage, "Normal");
      TextPage_AddOption(&LoRaCWModePage, "WakeUp");
      TextPage_AddOption(&LoRaCWModePage, "LowPower");
      TextPage_AddOption(&LoRaCWModePage, "SignalEnhancement");
    }
  }

  TextPage_Init(&HomePage, &OLED);

  SelectioneBar_BindTextPage(&SelectioneBar, HomePage.HeadPage);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart == LoRa.hUARTx)
  {
    LoRa.ReceiveSize += Size;

    if (LoRa.Mode == LoRaModeAT)
    {
      if (LoRa.ReceiveSize >= 4 && strncmp((char *) &LoRa.RxBuffer[LoRa.ReceiveSize - 4], "OK\r\n", 4) == 0)
      {
        LoRa.RxBuffer[LoRa.ReceiveSize] = '\0';
        LoRa.ReceiveOK = SET;
      } else
      {
        LoRa.ReceiveOK = RESET;
      }
    }
  }
}

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
        .A = 574.25,
        .B = -2.222,
        .Threshold,
    },
    {
        .Data = MQ3Data,
        .Length = MQ3DataLength,
        .A = 0.4091,
        .B = 0.652,
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
    .Config =
        {
            .BaudRate = LoRaBaudRate115200,
            .Parity = LoRaParityNone,
            .Address = 0x0000,
            .Channel = 23,
            .WLRate = LoRaWLRate19_2Kbps,
            .TPower = LoRaTPower20dBm,
            .WLTime = LoRaWLTime1s,
            .TMode = LoRaTModeTransparentTransmission,
            .CWMode = LoRaCWModeNormal,
        },
};

#define PageCount 4
uint8_t TextPageIndex, TextPageCount = PageCount;
TextPage_t *TextPages[PageCount];

Message_t Message;
Controller_t Controller;

void OLED_Info(OLED_t *OLED, char *Message)
{
  OLED_ClearBuffer(OLED);
  OLED_PrintfCenter(OLED, Message);
  OLED_SendBuffer(OLED);
  vTaskDelay(pdMS_TO_TICKS(500));
}

void Application_Init(void)
{
  OLED_Init(&OLED);

  OLED_Info(&OLED, "LoRa Connecting...");
  LoRa_StartIdleIT(&LoRa);
  LoRa_ATMode(&LoRa);
  LoRa_ApplyConfig(&LoRa, &LoRa.Config);
  LoRa_ReadConfig(&LoRa);
  LoRa_CommunicationMode(&LoRa);
  OLED_Info(&OLED, "LoRa Connected");

  OLED_Info(&OLED, "Encoder Initing...");
  Encoder_Start(&Encoder);
  OLED_Info(&OLED, "Encoder Initialized");

  OLED_Info(&OLED, "Sampler Initing...");
  Sampler_Start_DMA_TIM_IT(&Sampler);
  OLED_Info(&OLED, "Sampler Initialized");

  static TextPage_t MQxSensorPage = TextPage_NavigationPage("MQxSensor");
  MQxSensorPage.ShowCallback = TextPage_ShowMQxPageCallback;

  {
    static TextPage_t BackPage = TextPage_NextPage(">");
    TextPage_AddLowerPage(&MQxSensorPage, &BackPage);

    static TextPage_t MQ2TextPage = TextPage_MQxPage("MQ2");
    TextPage_AddLowerPage(&MQxSensorPage, &MQ2TextPage);

    static TextPage_t MQ3TextPage = TextPage_MQxPage("MQ3");
    TextPage_AddLowerPage(&MQxSensorPage, &MQ3TextPage);
  }

  static TextPage_t MQ2Page = TextPage_MQxPage("MQ2");
  MQ2Page.Index = 1;
  {
    static TextPage_t BackPage = TextPage_NextPage(">");
    TextPage_AddLowerPage(&MQ2Page, &BackPage);

    static TextPage_t MQ2ChartPage;
    MQ2ChartPage = (TextPage_t) TextPage_ChartPage("MQ2");
    TextPage_AddLowerPage(&MQ2Page, &MQ2ChartPage);
  }

  static TextPage_t MQ3Page = TextPage_MQxPage("MQ3");
  MQ3Page.Index = 2;
  {
    static TextPage_t BackPage = TextPage_NextPage(">");
    TextPage_AddLowerPage(&MQ3Page, &BackPage);

    static TextPage_t MQ3ChartPage;
    MQ3ChartPage = (TextPage_t) TextPage_ChartPage("MQ3");
    TextPage_AddLowerPage(&MQ3Page, &MQ3ChartPage);
  }

  static TextPage_t LoRaPage = TextPage_NavigationPage("LoRa");
  LoRaPage.ShowCallback = TextPage_ShowLoRaPage;
  {
    static TextPage_t BackPage = TextPage_NextPage(">");
    BackPage.RotationCallback = TextPage_CursorCallback;
    TextPage_AddLowerPage(&LoRaPage, &BackPage);

    static TextPage_t IDPage = TextPage_EmptyPage("ID");
    IDPage.IntParameterPtr = &Controller.ID;
    IDPage.RotationCallback = TextPage_CursorCallback;
    TextPage_AddLowerPage(&LoRaPage, &IDPage);

    static TextPage_t OnlinePage = TextPage_EmptyPage("Online");
    OnlinePage.IntParameterPtr = &Controller.Online;
    OnlinePage.RotationCallback = TextPage_CursorCallback;
    TextPage_AddLowerPage(&LoRaPage, &OnlinePage);

    static TextPage_t UploadPage = TextPage_EmptyPage("Upload");
    UploadPage.IntParameterPtr = &Controller.Upload;
    UploadPage.ClickCallback = TextPage_UploadCallback;
    UploadPage.RotationCallback = TextPage_CursorCallback;
    TextPage_AddLowerPage(&LoRaPage, &UploadPage);

    static TextPage_t RegisterPage = TextPage_EmptyPage("Register");
    RegisterPage.IntParameterPtr = &Controller.Register;
    RegisterPage.ClickCallback = TextPage_RegisterCallback;
    RegisterPage.RotationCallback = TextPage_CursorCallback;
    TextPage_AddLowerPage(&LoRaPage, &RegisterPage);
  }

  TextPages[0] = &MQxSensorPage;
  TextPages[1] = &MQ2Page;
  TextPages[2] = &MQ3Page;
  TextPages[3] = &LoRaPage;
  TextPage = TextPages[0];

  for (uint8_t i = 0; i < TextPageCount; i++)
  {
    TextPage_Init(TextPages[i], &OLED);
  }
  SelectioneBar_BindTextPage(&SelectioneBar, TextPages[0]->HeadPage);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart == LoRa.hUARTx)
  {
    LoRa.ReceiveSize += Size;

    if (LoRa.Mode == LoRaModeAT)
    {
      if (LoRa.ReceiveSize >= 4 &&
          strncmp((char *) &LoRa.RxBuffer[LoRa.ReceiveSize - 4], "OK\r\n", 4) == 0)
      {
        LoRa.RxBuffer[LoRa.ReceiveSize] = '\0';
        LoRa.ReceiveOK = SET;
      } else
      {
        LoRa.ReceiveOK = RESET;
      }
    } else if (LoRa.Mode == LoRaModeCommunication)
    {
      if (LoRa.RxBuffer[0] == 0xAA && LoRa.ReceiveSize >= 4 &&
          LoRa.ReceiveSize == (4 + LoRa.RxBuffer[3]))
      {
        Message.Head = LoRa.RxBuffer[0];
        Message.Type = LoRa.RxBuffer[1];
        Message.DeviceID = LoRa.RxBuffer[2];
        Message.Length = LoRa.RxBuffer[3];
        Message.Data = &LoRa.RxBuffer[4];
        Message.Tick = osKernelGetTickCount();

        LoRa.ReceiveMessage = SET;
      } else if (LoRa.RxBuffer[0] != 0xAA || LoRa.ReceiveSize >= 64)
      {
        LoRa_CLearReceive(&LoRa);
      } else
      {
        LoRa.ReceiveMessage = RESET;
      }
    }
  }
}

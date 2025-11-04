#include <stdarg.h>
#include <stdio.h>

#include "cmsis_os2.h"

#include "LoRa.h"

const char *LoRaBaudRateString[] = {
    "1200",
    "2400",
    "4800",
    "9600",
    "19200",
    "38400",
    "57600",
    "115200",
};

const char *LoRaParityString[] = {
    "None",
    "Even",
    "Odd",
};

const char *LoRaWLRateString[] = {
    "0.3Kbps",
    "1.2Kbps",
    "2.4Kbps",
    "4.8Kbps",
    "9.6Kbps",
    "19.2Kbps",
};

const char *LoRaTPowerString[] = {
    "11dBm",
    "14dBm",
    "17dBm",
    "20dBm",
};

const char *LoRaWLTimeString[] = {
    "1s",
    "2s",
};

const char *LoRaTModeString[] = {
    "Transparent",
    "Directed",
};

const char *LoRaCWModeString[] = {
    "Normal",
    "WakeUp",
    "LowPower",
    "SignalEnhancement",
};

void LoRa_StartIdleIT(LoRa_t *Self)
{
  HAL_UARTEx_ReceiveToIdle_IT(Self->hUARTx, Self->RxBuffer, sizeof(Self->RxBuffer));
}

void LoRa_Printf(LoRa_t *Self, const char *Format, ...)
{
  va_list Args;
  va_start(Args, Format);
  int32_t Length = vsnprintf((char *) Self->TxBuffer, sizeof(Self->TxBuffer), Format, Args);
  va_end(Args);

  if (Length > 0)
  {
    HAL_UART_Transmit(Self->hUARTx, Self->TxBuffer, Length, HAL_MAX_DELAY);
  }
}

void LoRa_ATMode(LoRa_t *Self)
{
  HAL_GPIO_WritePin(Self->AUX_Port, Self->AUX_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Self->MD0_Port, Self->MD0_Pin, GPIO_PIN_SET);
  osDelay(100);
  Self->Mode = LoRaModeAT;
}

void LoRa_CommunicationMode(LoRa_t *Self)
{
  HAL_GPIO_WritePin(Self->AUX_Port, Self->AUX_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Self->MD0_Port, Self->MD0_Pin, GPIO_PIN_RESET);
  osDelay(100);
  Self->Mode = LoRaModeCommunication;
}

void LoRa_CLearReceive(LoRa_t *Self)
{
  Self->ReceiveSize = 0;
  Self->ReceiveOK = RESET;
}

ErrorStatus LoRa_WaitForOK(LoRa_t *Self)
{
  uint32_t Tick = osKernelGetTickCount();

  while (Self->ReceiveOK != SET)
  {
    if (osKernelGetTickCount() - Tick > 1000)
    {
      LoRa_CLearReceive(Self);

      return ERROR;
    }

    osDelay(1);
  }

  LoRa_CLearReceive(Self);

  return SUCCESS;
}

#define LoRa_SendATCommandWithArgs(Self, Command, ...) \
  do                                                   \
  {                                                    \
    uint8_t RetryCount = 0;                            \
    ErrorStatus Status = ERROR;                        \
    while (Status == ERROR)                            \
    {                                                  \
      LoRa_Printf(Self, Command, __VA_ARGS__);         \
      Status = LoRa_WaitForOK(Self);                   \
      if (RetryCount >= 10)                            \
      {                                                \
        LoRa_CLearReceive(Self);                       \
        Status = ERROR;                                \
        break;                                         \
      }                                                \
      RetryCount++;                                    \
    }                                                  \
  } while (0)

ErrorStatus LoRa_SendATCommand(LoRa_t *Self, const char *Command)
{
  uint8_t RetryCount = 0;
  ErrorStatus Status = ERROR;
  while (Status == ERROR)
  {
    LoRa_Printf(Self, Command);
    Status = LoRa_WaitForOK(Self);

    if (RetryCount >= 10)
    {
      Status = ERROR;
      break;
    }
    RetryCount++;
  }

  return Status;
}


void LoRa_EnableEcho(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "ATE1\r\n");
}

void LoRa_DisableEcho(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "ATE0\r\n");
}

LoRa_BaudRate LoRa_ReadBaudRate(LoRa_t *Self)
{
  ErrorStatus Status = LoRa_SendATCommand(Self, "AT+UART?\r\n");

  uint8_t BaudRate;
  if (Status == SUCCESS)
  {
    BaudRate = Self->RxBuffer[6] - '0';
    // sscanf((char *) Self->RxBuffer, "+UART:%hhu,%hhu\r\n", &BaudRate, &Parity);
  } else
  {
    BaudRate = Self->Config.BaudRate;
  }

  return (LoRa_BaudRate) BaudRate;
}

LoRa_Parity LoRa_ReadParity(LoRa_t *Self)
{
  ErrorStatus Status = LoRa_SendATCommand(Self, "AT+UART?\r\n");

  uint8_t Parity;
  if (Status == SUCCESS)
  {
    Parity = Self->RxBuffer[8] - '0';
    // sscanf((char *) Self->RxBuffer, "+UART:%hhu,%hhu\r\n", &BaudRate, &Parity);
  } else
  {
    Parity = Self->Config.Parity;
  }

  return (LoRa_Parity) Parity;
}

uint8_t Hex2Dec(const char *Hex)
{
  uint8_t Result = 0;
  uint8_t Low = Hex[1];
  uint8_t High = Hex[0];

  if (High >= '0' && High <= '9')
  {
    Result += (High - '0') * 16;
  } else if (High >= 'A' && High <= 'F')
  {
    Result += (High - 'A' + 10) * 16;
  } else if (High >= 'a' && High <= 'f')
  {
    Result += (High - 'a' + 10) * 16;
  }

  if (Low >= '0' && Low <= '9')
  {
    Result += Low - '0';
  } else if (Low >= 'A' && Low <= 'F')
  {
    Result += Low - 'A' + 10;
  } else if (Low >= 'a' && Low <= 'f')
  {
    Result += Low - 'a' + 10;
  }

  return Result;
}

uint16_t LoRa_ReadAddress(LoRa_t *Self)
{
  ErrorStatus Status = LoRa_SendATCommand(Self, "AT+ADDR?\r\n");

  uint16_t Address;
  if (Status == SUCCESS)
  {
    uint8_t High = Hex2Dec((char *) &Self->RxBuffer[6]);
    uint8_t Low = Hex2Dec((char *) &Self->RxBuffer[9]);
    // sscanf((char *) Self->RxBuffer, "+ADDR:%2hhX,%2hhX\r\n", &High, &Low);
    Address = High << 8 | Low;
  } else
  {
    Address = Self->Config.Address;
  }

  return Address;
}

uint8_t LoRa_ReadChannel(LoRa_t *Self)
{
  ErrorStatus Status = LoRa_SendATCommand(Self, "AT+WLRATE?\r\n");

  uint8_t Channel = 0;
  if (Status == SUCCESS)
  {
    if (Self->RxBuffer[9] == ',')
    {
      Channel = Self->RxBuffer[8] - '0';
    } else if (Self->RxBuffer[10] == ',')
    {
      Channel = (Self->RxBuffer[8] - '0') * 10 + (Self->RxBuffer[9] - '0');
    }
    // sscanf((char *) Self->RxBuffer, "+WLRATE:%hhu,%hhu\r\n", &Channel, &WLRate);
  } else
  {
    Channel = Self->Config.Channel;
  }

  return Channel;
}

LoRa_WLRate LoRa_ReadWLRate(LoRa_t *Self)
{
  ErrorStatus Status = LoRa_SendATCommand(Self, "AT+WLRATE?\r\n");

  uint8_t WLRate = 0;
  if (Status == SUCCESS)
  {
    if (Self->RxBuffer[9] == ',')
    {
      WLRate = Self->RxBuffer[10] - '0';
    } else if (Self->RxBuffer[10] == ',')
    {
      WLRate = Self->RxBuffer[11] - '0';
    }
    // sscanf((char *) Self->RxBuffer, "+WLRATE:%hhu,%hhu\r\n", &Channel, &WLRate);
  } else
  {
    WLRate = Self->Config.WLRate;
  }

  return (LoRa_WLRate) WLRate;
}

LoRa_TPower LoRa_ReadTPower(LoRa_t *Self)
{
  ErrorStatus Status = LoRa_SendATCommand(Self, "AT+TPOWER?\r\n");

  uint8_t TPower;
  if (Status == SUCCESS)
  {
    TPower = Self->RxBuffer[8] - '0';
    // sscanf((char *) Self->RxBuffer, "+TPOWER:%hhu\r\n", &TPower);
  } else
  {
    TPower = Self->Config.TPower;
  }

  return (LoRa_TPower) TPower;
}

LoRa_WLTime LoRa_ReadWLTime(LoRa_t *Self)
{
  ErrorStatus Status = LoRa_SendATCommand(Self, "AT+WLTIME?\r\n");

  uint8_t WLTime;
  if (Status == SUCCESS)
  {
    WLTime = Self->RxBuffer[8] - '0';
    // sscanf((char *) Self->RxBuffer, "+WLTIME:%hhu\r\n", &WLTime);
  } else
  {
    WLTime = Self->Config.WLTime;
  }

  return (LoRa_WLTime) WLTime;
}

LoRa_TMode LoRa_ReadTMode(LoRa_t *Self)
{
  ErrorStatus Status = LoRa_SendATCommand(Self, "AT+TMODE?\r\n");

  uint8_t TMode;
  if (Status == SUCCESS)
  {
    TMode = Self->RxBuffer[7] - '0';
    // sscanf((char *) Self->RxBuffer, "+TMODE:%hhu\r\n", &TMode);
  } else
  {
    TMode = Self->Config.TMode;
  }

  return (LoRa_TMode) TMode;
}

LoRa_CWMode LoRa_ReadCWMode(LoRa_t *Self)
{
  ErrorStatus Status = LoRa_SendATCommand(Self, "AT+CWMODE?\r\n");

  uint8_t CWMode;
  if (Status == SUCCESS)
  {
    CWMode = Self->RxBuffer[8] - '0';
    // sscanf((char *) Self->RxBuffer, "+CWMODE:%hhu\r\n", &CWMode);
  } else
  {
    CWMode = Self->Config.CWMode;
  }

  return (LoRa_CWMode) CWMode;
}

void LoRa_SetBaudRateParity(LoRa_t *Self, LoRa_BaudRate BaudRate, LoRa_Parity Parity)
{
  Self->Config.BaudRate = BaudRate;
  Self->Config.Parity = Parity;

  LoRa_SendATCommandWithArgs(Self, "AT+UART=%d,%d\r\n", BaudRate, Parity);
}

void LoRa_SetAddress(LoRa_t *Self, uint16_t Address)
{
  Self->Config.Address = Address;

  LoRa_SendATCommandWithArgs(Self, "AT+ADDR=%02X,%02X\r\n", (uint8_t) (Address >> 8 & 0xFF), (uint8_t) (Address & 0xFF));
}

void LoRa_SetChannelWLRate(LoRa_t *Self, uint8_t Channel, LoRa_WLRate WLRate)
{
  Self->Config.Channel = Channel;
  Self->Config.WLRate = WLRate;

  LoRa_SendATCommandWithArgs(Self, "AT+WLRATE=%d,%d\r\n", Channel, WLRate);
}

void LoRa_SetTPower(LoRa_t *Self, LoRa_TPower TPower)
{
  Self->Config.TPower = TPower;

  LoRa_SendATCommandWithArgs(Self, "AT+TPOWER=%d\r\n", TPower);
}

void LoRa_SetWLTime(LoRa_t *Self, LoRa_WLTime WLTime)
{
  Self->Config.WLTime = WLTime;

  LoRa_SendATCommandWithArgs(Self, "AT+WLTIME=%d\r\n", WLTime);
}

void LoRa_SetTMode(LoRa_t *Self, LoRa_TMode TMode)
{
  Self->Config.TMode = TMode;

  LoRa_SendATCommandWithArgs(Self, "AT+TMODE=%d\r\n", TMode);
}

void LoRa_SetCWMode(LoRa_t *Self, LoRa_CWMode CWMode)
{
  Self->Config.CWMode = CWMode;

  LoRa_SendATCommandWithArgs(Self, "AT+CWMODE=%d\r\n", CWMode);
}

LoRaConfig_t *LoRa_ReadConfig(LoRa_t *Self)
{
  LoRa_DisableEcho(Self);

  Self->Config.BaudRate = LoRa_ReadBaudRate(Self);
  Self->Config.Parity = LoRa_ReadParity(Self);
  Self->Config.Address = LoRa_ReadAddress(Self);
  Self->Config.Channel = LoRa_ReadChannel(Self);
  Self->Config.WLRate = LoRa_ReadWLRate(Self);
  Self->Config.TPower = LoRa_ReadTPower(Self);
  Self->Config.WLTime = LoRa_ReadWLTime(Self);
  Self->Config.TMode = LoRa_ReadTMode(Self);
  Self->Config.CWMode = LoRa_ReadCWMode(Self);

  Self->ConfigArray[0] = Self->Config.BaudRate + 1;
  Self->ConfigArray[1] = Self->Config.Parity + 1;
  Self->ConfigArray[2] = Self->Config.Address + 1;
  Self->ConfigArray[3] = Self->Config.Channel + 1;
  Self->ConfigArray[4] = Self->Config.WLRate + 1;
  Self->ConfigArray[5] = Self->Config.TPower + 1;
  Self->ConfigArray[6] = Self->Config.WLTime + 1;
  Self->ConfigArray[7] = Self->Config.TMode + 1;
  Self->ConfigArray[8] = Self->Config.CWMode + 1;

  return &Self->Config;
}

void LoRa_ApplyConfig(LoRa_t *Self, LoRaConfig_t *Config)
{
  Self->Config = *Config;

  LoRa_DisableEcho(Self);

  LoRa_SetBaudRateParity(Self, Config->BaudRate, Config->Parity);
  LoRa_SetAddress(Self, Config->Address);
  LoRa_SetChannelWLRate(Self, Config->Channel, Config->WLRate);
  LoRa_SetTPower(Self, Config->TPower);
  LoRa_SetWLTime(Self, Config->WLTime);
  LoRa_SetTMode(Self, Config->TMode);
  LoRa_SetCWMode(Self, Config->CWMode);
}

void LoRa_LoadConfigFromArray(LoRa_t *Self, int32_t *ConfigArray)
{
  for (uint8_t i = 0; i < 9; i++)
  {
    Self->ConfigArray[i] = ConfigArray[i];
  }

  Self->Config.BaudRate = Self->ConfigArray[0] - 1;
  Self->Config.Parity = Self->ConfigArray[1] - 1;
  Self->Config.Address = Self->ConfigArray[2] - 1;
  Self->Config.Channel = Self->ConfigArray[3] - 1;
  Self->Config.WLRate = Self->ConfigArray[4] - 1;
  Self->Config.TPower = Self->ConfigArray[5] - 1;
  Self->Config.WLTime = Self->ConfigArray[6] - 1;
  Self->Config.TMode = Self->ConfigArray[7] - 1;
  Self->Config.CWMode = Self->ConfigArray[8] - 1;

  LoRa_ApplyConfig(Self, &Self->Config);
}

void LoRa_DumpConfigToArray(LoRa_t *Self, int32_t *ConfigArray)
{
  LoRa_ReadConfig(Self);

  for (uint8_t i = 0; i < 9; i++)
  {
    ConfigArray[i] = Self->ConfigArray[i];
  }
}

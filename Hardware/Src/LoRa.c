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
    "1.2Kbps",
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
    "TransparentTransmission",
    "DirectedTransmission",
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

void LoRa_WaitForOK(LoRa_t *Self)
{
  while (Self->ReceiveOK != SET)
  {
    osDelay(1);
  }
  LoRa_CLearReceive(Self);
}

#define LoRa_SendATCommandWithArgs(Self, Command, ...) \
  do                                                   \
  {                                                    \
    LoRa_Printf(Self, Command, __VA_ARGS__);           \
    LoRa_WaitForOK(Self);                              \
  } while (0)

#define LoRa_SendATCommand(Self, Command) \
  do                                      \
  {                                       \
    LoRa_Printf(Self, Command);           \
    LoRa_WaitForOK(Self);                 \
  } while (0)

void LoRa_EnableEcho(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "ATE1\r\n");
}

void LoRa_DisableEcho(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "ATE0\r\n");
}

void LoRa_ApplyConfig(LoRa_t *Self, LoRaConfig_t *Config)
{
  LoRa_DisableEcho(Self);
  LoRa_SetBaudRateParity(Self, Config->BaudRate, Config->Parity);
  LoRa_SetAddress(Self, Config->Address);
  LoRa_SetChannelWLRate(Self, Config->Channel, Config->WLRate);
  LoRa_SetTPower(Self, Config->TPower);
  LoRa_SetWLTime(Self, Config->WLTime);
  LoRa_SetTMode(Self, Config->TMode);
  LoRa_SetCWMode(Self, Config->CWMode);
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
  LoRa_ReadBaudRateParity(Self);
  LoRa_ReadAddress(Self);
  LoRa_ReadChannelWLRate(Self);
  LoRa_ReadTPower(Self);
  LoRa_ReadWLTime(Self);
  LoRa_ReadTMode(Self);
  LoRa_ReadCWMode(Self);

  return &Self->Config;
}

void LoRa_ReadBaudRateParity(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "AT+UART?\r\n");

  uint8_t BaudRate = Self->RxBuffer[6] - '0';
  uint8_t Parity = Self->RxBuffer[8] - '0';
  // sscanf((char *) Self->RxBuffer, "+UART:%hhu,%hhu\r\n", &BaudRate, &Parity);
  Self->Config.BaudRate = (LoRa_BaudRate) BaudRate;
  Self->Config.Parity = (LoRa_Parity) Parity;
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

void LoRa_ReadAddress(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "AT+ADDR?\r\n");

  uint8_t High = Hex2Dec((char *) &Self->RxBuffer[6]);
  uint8_t Low = Hex2Dec((char *) &Self->RxBuffer[9]);
  // sscanf((char *) Self->RxBuffer, "+ADDR:%2hhX,%2hhX\r\n", &High, &Low);
  Self->Config.Address = High << 8 | Low;
}

void LoRa_ReadChannelWLRate(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "AT+WLRATE?\r\n");

  uint8_t Channel = 0;
  uint8_t WLRate = 0;

  if (Self->RxBuffer[9] == ',')
  {
    Channel = Self->RxBuffer[8] - '0';
    WLRate = Self->RxBuffer[10] - '0';
  } else if (Self->RxBuffer[10] == ',')
  {
    Channel = (Self->RxBuffer[8] - '0') * 10 + (Self->RxBuffer[9] - '0');
    WLRate = Self->RxBuffer[11] - '0';
  }
  // sscanf((char *) Self->RxBuffer, "+WLRATE:%hhu,%hhu\r\n", &Channel, &WLRate);

  Self->Config.Channel = Channel;
  Self->Config.WLRate = (LoRa_WLRate) WLRate;
}

void LoRa_ReadTPower(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "AT+TPOWER?\r\n");

  uint8_t TPower = Self->RxBuffer[8] - '0';
  // sscanf((char *) Self->RxBuffer, "+TPOWER:%hhu\r\n", &TPower);
  Self->Config.TPower = (LoRa_TPower) TPower;
}

void LoRa_ReadWLTime(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "AT+WLTIME?\r\n");

  uint8_t WLTime = Self->RxBuffer[8] - '0';
  // sscanf((char *) Self->RxBuffer, "+WLTIME:%hhu\r\n", &WLTime);
  Self->Config.WLTime = (LoRa_WLTime) WLTime;
}

void LoRa_ReadTMode(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "AT+TMODE?\r\n");

  uint8_t TMode = Self->RxBuffer[7] - '0';
  // sscanf((char *) Self->RxBuffer, "+TMODE:%hhu\r\n", &TMode);
  Self->Config.TMode = (LoRa_TMode) TMode;
}

void LoRa_ReadCWMode(LoRa_t *Self)
{
  LoRa_SendATCommand(Self, "AT+CWMODE?\r\n");

  uint8_t CWMode = Self->RxBuffer[8] - '0';
  // sscanf((char *) Self->RxBuffer, "+CWMODE:%hhu\r\n", &CWMode);
  Self->Config.CWMode = (LoRa_CWMode) CWMode;
}

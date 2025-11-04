#ifndef __LORA_H__
#define __LORA_H__

#include "stm32f1xx_hal.h"

typedef enum
{
  LoRaBaudRate1200,
  LoRaBaudRate2400,
  LoRaBaudRate4800,
  LoRaBaudRate9600,
  LoRaBaudRate19200,
  LoRaBaudRate38400,
  LoRaBaudRate57600,
  LoRaBaudRate115200,
} LoRa_BaudRate;

typedef enum
{
  LoRaParityNone,
  LoRaParityEven,
  LoRaParityOdd,
} LoRa_Parity;

typedef enum
{
  LoRaWLRate0_3Kbps,
  LoRaWLRate1_2Kbps,
  LoRaWLRate2_4Kbps,
  LoRaWLRate4_8Kbps,
  LoRaWLRate9_6Kbps,
  LoRaWLRate19_2Kbps,
} LoRa_WLRate;

typedef enum
{
  LoRaTPower11dBm,
  LoRaTPower14dBm,
  LoRaTPower17dBm,
  LoRaTPower20dBm,
} LoRa_TPower;

typedef enum
{
  LoRaWLTime1s,
  LoRaWLTime2s,
} LoRa_WLTime;

typedef enum
{
  LoRaTModeTransparentTransmission,
  LoRaTModeDirectedTransmission
} LoRa_TMode;

typedef enum
{
  LoRaCWModeNormal,
  LoRaCWModeWakeUp,
  LoRaCWModeLowPower,
  LoRaCWModeSignalEnhancement,
} LoRa_CWMode;

extern const char *LoRaBaudRateString[];
extern const char *LoRaParityString[];
extern const char *LoRaWLRateString[];
extern const char *LoRaTPowerString[];
extern const char *LoRaWLTimeString[];
extern const char *LoRaTModeString[];
extern const char *LoRaCWModeString[];

typedef struct
{
  LoRa_BaudRate BaudRate;
  LoRa_Parity Parity;
  uint16_t Address;
  uint8_t Channel;
  LoRa_WLRate WLRate;
  LoRa_TPower TPower;
  LoRa_WLTime WLTime;
  LoRa_TMode TMode;
  LoRa_CWMode CWMode;
} LoRaConfig_t;

typedef enum
{
  LoRaModeAT,
  LoRaModeCommunication,
} LoRa_Mode;

typedef struct
{
  UART_HandleTypeDef *hUARTx;
  GPIO_TypeDef *AUX_Port;
  uint32_t AUX_Pin;
  GPIO_TypeDef *MD0_Port;
  uint32_t MD0_Pin;

  LoRa_Mode Mode;

  uint8_t ReceiveSize;
  FlagStatus ReceiveOK;
  uint8_t RxBuffer[128];
  uint8_t TxBuffer[128];

  LoRaConfig_t Config;
} LoRa_t;

void LoRa_StartIdleIT(LoRa_t *Self);
void LoRa_Printf(LoRa_t *Self, const char *Format, ...);

void LoRa_ATMode(LoRa_t *Self);
void LoRa_CommunicationMode(LoRa_t *Self);

void LoRa_EnableEcho(LoRa_t *Self);
void LoRa_DisableEcho(LoRa_t *Self);

void LoRa_ApplyConfig(LoRa_t *Self, LoRaConfig_t *Config);
void LoRa_SetBaudRateParity(LoRa_t *Self, LoRa_BaudRate BaudRate, LoRa_Parity Parity);
void LoRa_SetAddress(LoRa_t *Self, uint16_t Address);
void LoRa_SetChannelWLRate(LoRa_t *Self, uint8_t Channel, LoRa_WLRate WLRate);
void LoRa_SetTPower(LoRa_t *Self, LoRa_TPower TPower);
void LoRa_SetWLTime(LoRa_t *Self, LoRa_WLTime WLTime);
void LoRa_SetTMode(LoRa_t *Self, LoRa_TMode TMode);
void LoRa_SetCWMode(LoRa_t *Self, LoRa_CWMode CWMode);

LoRaConfig_t *LoRa_ReadConfig(LoRa_t *Self);
void LoRa_ReadBaudRateParity(LoRa_t *Self);
void LoRa_ReadAddress(LoRa_t *Self);
void LoRa_ReadChannelWLRate(LoRa_t *Self);
void LoRa_ReadTPower(LoRa_t *Self);
void LoRa_ReadWLTime(LoRa_t *Self);
void LoRa_ReadTMode(LoRa_t *Self);
void LoRa_ReadCWMode(LoRa_t *Self);

#endif

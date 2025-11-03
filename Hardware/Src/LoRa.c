#include <stdarg.h>
#include <stdio.h>

#include "cmsis_os2.h"

#include "LoRa.h"

void LoRa_StartIdleIT(LoRa_t *Self)
{
  HAL_UARTEx_ReceiveToIdle_IT(Self->hUARTx, Self->RxBuffer, sizeof(Self->RxBuffer));
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

#define LoRa_SendATCommand(Command, ...)     \
  do                                         \
  {                                          \
    LoRa_Printf(Self, Command, __VA_ARGS__); \
    LoRa_WaitForOK(Self);                    \
  } while (0)

void LoRa_EnableEcho(LoRa_t *Self)
{
  LoRa_SendATCommand("ATE1\r\n", NULL);
}

void LoRa_DisableEcho(LoRa_t *Self)
{
  LoRa_SendATCommand("ATE0\r\n", NULL);
}

void LoRa_SetBaudRateParity(LoRa_t *Self, LoRa_BaudRate BaudRate, LoRa_Parity Parity)
{
  LoRa_SendATCommand("AT+UART=%d,%d\r\n", BaudRate, Parity);
}

void LoRa_SetAddress(LoRa_t *Self, uint16_t Address)
{
  LoRa_SendATCommand("AT+ADDR=%02d,%02d\r\n", (uint8_t) (Address >> 8 & 0xFF), (uint8_t) (Address & 0xFF));
}

void LoRa_SetChannelWLRate(LoRa_t *Self, uint8_t Channel, LoRa_WLRate WLRate)
{
  LoRa_SendATCommand("AT+WLRATE=%d,%d\r\n", Channel, WLRate);
}

void LoRa_SetTPower(LoRa_t *Self, LoRa_TPower TPower)
{
  LoRa_SendATCommand("AT+TPOWER=%d\r\n", TPower);
}

void LoRa_SetWLTime(LoRa_t *Self, LoRa_WLTime WLTime)
{
  LoRa_SendATCommand("AT+WLTIME=%d\r\n", WLTime);
}

void LoRa_SetTMode(LoRa_t *Self, LoRa_TMode TMode)
{
  LoRa_SendATCommand("AT+TMODE=%d\r\n", TMode);
}

void LoRa_SetCWMode(LoRa_t *Self, LoRa_CWMode CWMode)
{
  LoRa_SendATCommand("AT+CWMODE=%d\r\n", CWMode);
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

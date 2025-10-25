#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "stm32f1xx_hal.h"

typedef struct
{
  UART_HandleTypeDef *hUARTx;

  uint8_t TxBuffer[128];
} Serial_t;

void Serial_Printf(Serial_t *Self, const char *Format, ...);

#endif

#include <stdarg.h>
#include <stdio.h>

#include "Serial.h"

void Serial_Printf(Serial_t *Self, const char *Format, ...)
{
  va_list Args;
  va_start(Args, Format);
  int32_t Length = vsnprintf((char *) Self->TxBuffer, sizeof(Self->TxBuffer), Format, Args);
  va_end(Args);

  if (Length > 0)
  {
    HAL_UART_Transmit(Self->hUART, Self->TxBuffer, Length, HAL_MAX_DELAY);
  }
}

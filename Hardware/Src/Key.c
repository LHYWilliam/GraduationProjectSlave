#include "cmsis_os2.h"

#include "Key.h"

FlagStatus Key_IsPressing(Key_t *Self)
{
  return HAL_GPIO_ReadPin(Self->GPIOx, Self->Pin) == Self->ActiveState;
}

FlagStatus Key_WasPressed(Key_t *Self)
{
  if (Key_IsPressing(Self))
  {
    osDelay(10);
    while (Key_IsPressing(Self))
      ;
    osDelay(10);

    return SET;
  }

  return RESET;
}

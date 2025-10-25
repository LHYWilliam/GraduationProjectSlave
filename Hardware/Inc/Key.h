#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f1xx_hal.h"

typedef struct {
  GPIO_TypeDef *GPIOx;
  uint16_t Pin;
  GPIO_PinState ActiveState;
} Key_t;

FlagStatus Key_IsPressing(Key_t *Self);
FlagStatus Key_WasPressed(Key_t *Self);

#endif

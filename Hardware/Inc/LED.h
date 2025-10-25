#ifndef __LED_H__
#define __LED_H__

#include "stm32f1xx_hal.h"

typedef struct {
  GPIO_TypeDef *GPIOx;
  uint32_t Pin;
  GPIO_PinState ActiveState;
} LED_t;

void LED_On(LED_t *Self);
void LED_Off(LED_t *Self);
void LED_Toggle(LED_t *Self);

#endif

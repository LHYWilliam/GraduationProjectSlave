#include "LED.h"

void LED_On(LED_t *Self) {
  HAL_GPIO_WritePin(Self->GPIOx, Self->Pin, (GPIO_PinState)Self->Mode);
}

void LED_Off(LED_t *Self) {
  HAL_GPIO_WritePin(Self->GPIOx, Self->Pin, (GPIO_PinState)!Self->Mode);
}

void LED_Toggle(LED_t *Self) { HAL_GPIO_TogglePin(Self->GPIOx, Self->Pin); }

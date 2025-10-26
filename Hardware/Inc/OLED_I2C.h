#ifndef __OLED_I2C_H__
#define __OLED_I2C_H__

#include "stm32f1xx_hal.h"

#include "OLED.h"

void OLED_I2C_WriteDatas(OLED_t *Self, uint8_t *Datas, uint16_t Length);
void OLED_I2C_WriteCommand(OLED_t *Self, uint8_t Command);
void OLED_I2C_WriteCommands(OLED_t *Self, uint8_t *Commands, uint16_t Length);

#endif

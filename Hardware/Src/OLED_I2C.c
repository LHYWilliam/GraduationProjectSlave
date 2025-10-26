#include "cmsis_os2.h"

#include "OLED_I2C.h"

void OLED_I2C_WriteDatas(OLED_t *Self, uint8_t *Datas, uint16_t Length)
{
  HAL_I2C_Mem_Write_DMA(Self->hI2Cx, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, Datas, Length);

  while (HAL_I2C_GetState(Self->hI2Cx) != HAL_I2C_STATE_READY)
  {
    osThreadYield();
  }
}

void OLED_I2C_WriteCommand(OLED_t *Self, uint8_t Command)
{
  HAL_I2C_Mem_Write(Self->hI2Cx, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &Command, 1, HAL_MAX_DELAY);
}

void OLED_I2C_WriteCommands(OLED_t *Self, uint8_t *Commands, uint16_t Length)
{
  HAL_I2C_Mem_Write(Self->hI2Cx, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, Commands, Length, HAL_MAX_DELAY);
}

#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "stm32f1xx_hal.h"

typedef struct
{
  TIM_HandleTypeDef *hTIMx;

  uint16_t Count;
  int16_t Speed;
} Encoder_t;

void Encoder_Start(Encoder_t *Self);
uint16_t Encoder_GetCount(Encoder_t *Self);
int16_t Encoder_GetSpeed(Encoder_t *Self);

#endif

#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include "stm32f1xx_hal.h"

typedef struct
{
  ADC_HandleTypeDef *hADCx;
  TIM_HandleTypeDef *hTIMx;

  uint16_t Index;
  uint16_t Length;
  uint16_t *Buffer;
} Sampler_t;


void Sampler_Start_DMA(Sampler_t *Self);
void Sampler_Start_DMA_TIM(Sampler_t *Self);
void Sampler_Start_DMA_TIM_IT(Sampler_t *Self);
uint16_t Sampler_GetValue(Sampler_t *Self, uint32_t Channel);


#endif

#include "Sampler.h"

void Sampler_Start_DMA(Sampler_t *Self)
{
  HAL_ADC_Start_DMA(Self->hADCx, (uint32_t *) Self->Buffer, Self->Length);
}

void Sampler_Start_DMA_TIM(Sampler_t *Self)
{
  HAL_TIM_Base_Start(Self->hTIMx);
  HAL_ADC_Start_DMA(Self->hADCx, (uint32_t *) Self->Buffer, Self->Length);
}

void Sampler_Start_DMA_TIM_IT(Sampler_t *Self)
{
  HAL_TIM_Base_Start_IT(Self->hTIMx);
  HAL_ADC_Start_DMA(Self->hADCx, (uint32_t *) Self->Buffer, Self->Length);
}

uint16_t Sampler_GetValue(Sampler_t *Self, uint32_t Channel)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  sConfig.Channel = Channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;

  HAL_ADC_ConfigChannel(Self->hADCx, &sConfig);

  HAL_ADC_Start(Self->hADCx);
  HAL_ADC_PollForConversion(Self->hADCx, HAL_MAX_DELAY);

  uint16_t value = HAL_ADC_GetValue(Self->hADCx);

  HAL_ADC_Stop(Self->hADCx);

  return value;
}

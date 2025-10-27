#include "Encoder.h"

void Encoder_Start(Encoder_t *Self)
{
  HAL_TIM_Encoder_Start(Self->hTIMx, TIM_CHANNEL_ALL);
}

uint16_t Encoder_GetCount(Encoder_t *Self)
{
  Self->Count = __HAL_TIM_GET_COUNTER(Self->hTIMx);

  return Self->Count;
}

int16_t Encoder_GetSpeed(Encoder_t *Self)
{
  uint16_t Count = __HAL_TIM_GET_COUNTER(Self->hTIMx);
  Self->Speed = (int16_t) Count - (int16_t) Self->Count;
  Self->Count = Count;

  return Self->Speed;
}

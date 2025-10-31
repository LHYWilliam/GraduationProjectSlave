#include "MQSensor.h"

void MQSensor_Update(MQSensor_t *Self, uint16_t Data)
{
  Self->Data[Self->Index] = Data;
  Self->Index = (Self->Index + 1) % Self->Length;


  switch (Self->State)
  {
  case MQSensorDanger:
    if (MQSensor_GetData(Self) < Self->Threshold - Self->Relaxation)
    {
      Self->State = MQSensorSafe;
    }
    break;

  case MQSensorSafe:
    if (MQSensor_GetData(Self) > Self->Threshold + Self->Relaxation)
    {
      Self->State = MQSensorDanger;
    }
    break;
  }
}

uint16_t MQSensor_GetData(MQSensor_t *Self)
{
  return Self->Data[(Self->Index + Self->Length - 1) % Self->Length];
}

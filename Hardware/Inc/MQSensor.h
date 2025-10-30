#ifndef __MQSENSOR_H__
#define __MQSENSOR_H__

#include "stm32f1xx_hal.h"

typedef struct
{
  uint16_t *Data;
  uint16_t Length;
  uint16_t Index;
} MQSensor_t;

void MQSensor_Update(MQSensor_t *Self, uint16_t Data);
uint16_t MQSensor_GetData(MQSensor_t *Self);

#endif

#ifndef __MQSENSOR_H__
#define __MQSENSOR_H__

#include "stm32f1xx_hal.h"

#include <math.h>

typedef enum
{
  MQSensorDanger,
  MQSensorSafe,
} MQSensor_State;

typedef struct
{
  uint16_t *Data;
  uint16_t Length;
  uint16_t Index;

  uint16_t Threshold;
  uint16_t Relaxation;
  MQSensor_State State;

  float A;
  float B;
} MQSensor_t;

void MQSensor_Update(MQSensor_t *Self, uint16_t Data);
uint16_t MQSensor_GetData(MQSensor_t *Self);
float MQSensor_CalculateMQ2PPM(uint16_t ADCValue);
float MQSensor_CalculateMQ3PPM(uint16_t ADCValue);
float MQSensor_CalculateMQ4PPM(uint16_t ADCValue);
float MQSensor_CalculateMQ5PPM(uint16_t ADCValue);

#endif

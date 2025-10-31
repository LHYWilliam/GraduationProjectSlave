#ifndef __PID_H__
#define __PID_H__

#include "stm32f1xx_hal.h"

typedef struct
{
  float Kp;
  float Ki;
  float Kd;
  float Integral;
  float LastError;
  uint32_t LastTime;
} PID_t;

void PID_Init(PID_t *Self, float Kp, float Ki, float Kd);
float PID_Calculate(PID_t *Self, float Error, uint32_t Time);

#endif

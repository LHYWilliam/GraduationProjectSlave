#include "PID.h"

void PID_Init(PID_t *Self, float Kp, float Ki, float Kd)
{
  Self->Kp = Kp;
  Self->Ki = Ki;
  Self->Kd = Kd;
  Self->Integral = 0.0f;
  Self->LastError = 0.0f;
  Self->LastTime = 0;
}

float PID_Calculate(PID_t *Self, float Error, uint32_t Time)
{
  float Output = Self->Kp * Error;

  float dt = (Time - Self->LastTime) / 1000.0f;
  if (dt <= 0)
  {
    dt = 0.01f;
  }

  if (Self->Ki)
  {
    Self->Integral += Error * dt;

    const float IntegralMax = 100.0f;
    if (Self->Integral > IntegralMax)
      Self->Integral = IntegralMax;
    if (Self->Integral < -IntegralMax)
      Self->Integral = -IntegralMax;
    Output += Self->Ki * Self->Integral;
  }

  if (Self->Kd)
  {
    float Derivative = (Error - Self->LastError) / dt;
    Output += Self->Kd * Derivative;
  }


  Self->LastError = Error;
  Self->LastTime = Time;

  return Output;
}

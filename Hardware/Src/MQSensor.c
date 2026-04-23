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

/**
 * 通用的电压计算辅助函数：将 ADC 读数转换为负载电阻 R_L 两端的电压 (V)
 * @param adcValue ADC 采样值 (0-4095)
 * @param vref 参考电压 (3.3V)
 * @param resolution ADC 分辨率 (12位，即4096)
 * @return 计算出的电压值 (V)
 */
float calculateVRL(uint16_t adcValue, float vref, uint16_t resolution)
{
  return vref * adcValue / (float) resolution;
}

// ========== 1. MQ-2 传感器 (检测液化石油气 LPG) ==========
/**
 * 计算 MQ-2 传感器对应的 LPG 浓度 (PPM)
 * @param adcValue ADC 采样值 (0-4095)
 * @return 估算的 LPG 浓度 (PPM)，浮点型
 * 
 * 注意：内置的 R0 值为 6.00，单位为 kΩ，这是一个常见的经验值。
 */
float MQSensor_CalculateMQ2PPM(uint16_t adcValue)
{
  // 工程经验参数 (针对液化石油气 LPG)
  const float RL = 5.0f;   // 负载电阻，常见为5kΩ
  const float R0 = 6.00f;  // 洁净空气中的传感器电阻 (kΩ)，这是一个经验值
  const float Vc = 5.0f;   // 模块供电电压
  const float a = 574.25f; // 回归系数 a
  const float b = -2.222f; // 回归系数 b

  float VRL = calculateVRL(adcValue, 3.3f, 4096.0f);

  // 计算传感器电阻 RS，单位与 RL 保持一致 (kΩ)
  float RS = (Vc - VRL) / VRL * RL;

  // 计算浓度 PPM
  float ppm = a * pow(RS / R0, b);

  return ppm;
}

// ========== 2. MQ-3 传感器 (检测酒精) ==========
/**
 * 计算 MQ-3 传感器对应的酒精浓度 (PPM)
 * @param adcValue ADC 采样值 (0-4095)
 * @return 估算的酒精浓度 (PPM)，浮点型
 * 
 * 注意：内置的 R0 值为 10.00，单位为 kΩ，这是一个常见的经验值。
 */
float MQSensor_CalculateMQ3PPM(uint16_t adcValue)
{
  // 工程经验参数 (酒精)
    const float RL = 1.0f;          // 负载电阻 1kΩ
    const float R0 = 10.00f;        // 洁净空气电阻 (kΩ)，经验值
    const float Vc = 5.0f;          // 供电电压 5V
    const float a = 4.8387f;        // 原系数 a
    const float b = -2.68f;         // 原系数 b

    float VRL = 3.3f * adcValue / 4096.0f;
    float RS = (Vc - VRL) / VRL * RL;
    float ppm = a * pow(RS / R0, b);

    return ppm;
}

// ========== 3. MQ-4 传感器 (检测甲烷 CH4) ==========
/**
 * 计算 MQ-4 传感器对应的甲烷浓度 (PPM)
 * @param adcValue ADC 采样值 (0-4095)
 * @return 估算的甲烷浓度 (PPM)，浮点型
 * 
 * 注意：内置的 R0 值为 12.00，单位为 kΩ，这是一个常见的经验值。
 */
float MQSensor_CalculateMQ4PPM(uint16_t adcValue)
{
  // 工程经验参数 (针对甲烷)
  const float RL = 5.0f;   // 负载电阻，常见为5kΩ
  const float R0 = 12.00f; // 洁净空气中的传感器电阻 (kΩ)，经验值
  const float Vc = 5.0f;   // 模块供电电压
  const float a = 1012.7f; // 回归系数 a
  const float b = -2.786f; // 回归系数 b

  float VRL = calculateVRL(adcValue, 3.3f, 4096.0f);
  float RS = (Vc - VRL) / VRL * RL;
  float ppm = a * pow(RS / R0, b);

  return ppm;
}

// ========== 4. MQ-5 传感器 (检测液化石油气 LPG) ==========
/**
 * 计算 MQ-5 传感器对应的液化石油气 (LPG) 浓度 (PPM)
 * @param adcValue ADC 采样值 (0-4095)
 * @return 估算的 LPG 浓度 (PPM)，浮点型
 * 
 * 注意：内置的 R0 值为 8.00，单位为 kΩ，这是一个常见的经验值。
 */
float MQSensor_CalculateMQ5PPM(uint16_t adcValue)
{
  // 工程经验参数 (针对液化石油气 LPG)
  const float RL = 5.0f;   // 负载电阻，常见为5kΩ
  const float R0 = 8.00f;  // 洁净空气中的传感器电阻 (kΩ)，经验值
  const float Vc = 5.0f;   // 模块供电电压
  const float a = 80.897f; // 回归系数 a
  const float b = -2.431f; // 回归系数 b

  float VRL = calculateVRL(adcValue, 3.3f, 4096.0f);
  float RS = (Vc - VRL) / VRL * RL;
  float ppm = a * pow(RS / R0, b);

  return ppm;
}
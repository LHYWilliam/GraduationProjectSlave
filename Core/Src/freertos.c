/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LEDTimer */
osTimerId_t LEDTimerHandle;
const osTimerAttr_t LEDTimer_attributes = {
  .name = "LEDTimer"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void OLED_Test(void);
void Sampler_Test(void);
void Encoder_Test(void);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void LEDTimerCode(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of LEDTimer */
  LEDTimerHandle = osTimerNew(LEDTimerCode, osTimerPeriodic, NULL, &LEDTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */

  osTimerStart(LEDTimerHandle, 100);

  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */

  UNUSED(argument);

  OLED_Init(&OLED);

  /* Infinite loop */
  for (;;)
  {
    OLED_Test();

    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* LEDTimerCode function */
void LEDTimerCode(void *argument)
{
  /* USER CODE BEGIN LEDTimerCode */

  UNUSED(argument);

  LED_Toggle(&BoardLED);

  // float Voltage1 = Sampler.Buffer[0] * 3.3 / 4095., Voltage2 = Sampler.Buffer[1] * 3.3 / 4095.;
  // Serial_Printf(&Serial, "[%.3f %.3f]\r\n", Voltage1, Voltage2);

  /* USER CODE END LEDTimerCode */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void OLED_Test(void)
{
  static int X = 0, Y = 0;

  X = 0, Y = 0;

  while (X < OLED.Width - 1)
  {
    OLED_ClearBuffer(&OLED);
    OLED_DrawLine(&OLED, 64, 32, X, Y);
    OLED_SendBuffer(&OLED);
    X++;

    osDelay(1);
  }

  while (Y < OLED.Height - 1)
  {
    OLED_ClearBuffer(&OLED);
    OLED_DrawLine(&OLED, 64, 32, X, Y);
    OLED_SendBuffer(&OLED);
    Y++;

    osDelay(1);
  }

  while (X > 0)
  {
    OLED_ClearBuffer(&OLED);
    OLED_DrawLine(&OLED, 64, 32, X, Y);
    OLED_SendBuffer(&OLED);
    X--;

    osDelay(1);
  }

  while (Y > 0)
  {
    OLED_ClearBuffer(&OLED);
    OLED_DrawLine(&OLED, 64, 32, X, Y);
    OLED_SendBuffer(&OLED);
    Y--;

    osDelay(1);
  }
}

void Sampler_Test(void)
{
  float Voltage1 = Sampler.Buffer[0] * 3.3 / 4095., Voltage2 = Sampler.Buffer[1] * 3.3 / 4095.;
  Serial_Printf(&Serial, "[%.3f %.3f]\r\n", Voltage1, Voltage2);

  osDelay(100);
}

void Encoder_Test(void)
{
  if (Key_IsPressing(&EncoderKey))
  {
    LED_On(&BoardLED);
  } else
  {
    LED_Off(&BoardLED);
  }

  int16_t Speed = Encoder_GetSpeed(&Encoder);
  uint16_t Count = Encoder_GetCount(&Encoder);
  Serial_Printf(&Serial, "[Count, Speed]: [%d, %d]\r\n", Count, Speed);

  osDelay(100);
}

/* USER CODE END Application */


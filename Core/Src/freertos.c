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

#include "Application.h"

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
/* Definitions for OLEDFlushTask */
osThreadId_t OLEDFlushTaskHandle;
const osThreadAttr_t OLEDFlushTask_attributes = {
  .name = "OLEDFlushTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for OLEDInteraction */
osThreadId_t OLEDInteractionHandle;
const osThreadAttr_t OLEDInteraction_attributes = {
  .name = "OLEDInteraction",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for LEDTimer */
osTimerId_t LEDTimerHandle;
const osTimerAttr_t LEDTimer_attributes = {
  .name = "LEDTimer"
};
/* Definitions for LoRaTimer */
osTimerId_t LoRaTimerHandle;
const osTimerAttr_t LoRaTimer_attributes = {
  .name = "LoRaTimer"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void OLED_Test(void);
void Sampler_Test(void);
void Encoder_Test(void);

/* USER CODE END FunctionPrototypes */

void OLEDFlushTaskCode(void *argument);
void OLEDInteractionTaskCode(void *argument);
void LEDTimerCallback(void *argument);
void LoRaTimerCallback(void *argument);

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
  LEDTimerHandle = osTimerNew(LEDTimerCallback, osTimerPeriodic, NULL, &LEDTimer_attributes);

  /* creation of LoRaTimer */
  LoRaTimerHandle = osTimerNew(LoRaTimerCallback, osTimerPeriodic, NULL, &LoRaTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */

  osTimerStart(LEDTimerHandle, 100);
  osTimerStart(LoRaTimerHandle, 1000);

  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of OLEDFlushTask */
  OLEDFlushTaskHandle = osThreadNew(OLEDFlushTaskCode, NULL, &OLEDFlushTask_attributes);

  /* creation of OLEDInteraction */
  OLEDInteractionHandle = osThreadNew(OLEDInteractionTaskCode, NULL, &OLEDInteraction_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_OLEDFlushTaskCode */
/**
  * @brief  Function implementing the OLEDFlushTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_OLEDFlushTaskCode */
void OLEDFlushTaskCode(void *argument)
{
  /* USER CODE BEGIN OLEDFlushTaskCode */

  UNUSED(argument);

  Application_Init();

  /* Infinite loop */
  // uint32_t Tick = osKernelGetTickCount();
  for (;;)
  {
    // LoRa_Printf(&LoRa, "%d\r\n", osKernelGetTickCount() - Tick);
    // Tick = osKernelGetTickCount();

    OLED_ClearBuffer(&OLED);

    TextPage->UpdateCallback(TextPage, &OLED);
    SelectioneBar_Update(&SelectioneBar);

    TextPage->ShowCallback(TextPage, &OLED);
    OLED_ShowSelectioneBar(&OLED, &SelectioneBar);

    OLED_SendBuffer(&OLED);

    osDelay(1);
  }
  /* USER CODE END OLEDFlushTaskCode */
}

/* USER CODE BEGIN Header_OLEDInteractionTaskCode */
/**
* @brief Function implementing the OLEDInteraction thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_OLEDInteractionTaskCode */
void OLEDInteractionTaskCode(void *argument)
{
  /* USER CODE BEGIN OLEDInteractionTaskCode */

  UNUSED(argument);

  uint32_t Tick = osKernelGetTickCount();

  /* Infinite loop */
  for (;;)
  {
    Tick = osKernelGetTickCount();

    int16_t Speed = Encoder_GetSpeed(&Encoder);
    if (Speed >= 3)
    {
      TextPage->LowerPages->RotationCallback(TextPage, &SelectioneBar, RotationDown);
    } else if (Speed <= -3)
    {
      TextPage->LowerPages->RotationCallback(TextPage, &SelectioneBar, RotationUp);
    }

    if (Key_WasPressed(&EncoderKey))
    {
      TextPage->LowerPages->ClickCallback(&TextPage, &SelectioneBar);
    }

    osDelayUntil(Tick + 100);
  }
  /* USER CODE END OLEDInteractionTaskCode */
}

/* LEDTimerCallback function */
void LEDTimerCallback(void *argument)
{
  /* USER CODE BEGIN LEDTimerCallback */

  UNUSED(argument);

  LED_Toggle(&BoardLED);

  /* USER CODE END LEDTimerCallback */
}

/* LoRaTimerCallback function */
void LoRaTimerCallback(void *argument)
{
  /* USER CODE BEGIN LoRaTimerCallback */

  UNUSED(argument);

  if (LoRa.Mode != LoRaModeCommunication)
  {
    return;
  }

  LoRa_Printf(
      &LoRa,
      "MQ2: [%.3f] | MQ3: [%.3f]\r\n",
      ADCToVoltage(MQSensor_GetData(&MQxSensor[0])),
      ADCToVoltage(MQSensor_GetData(&MQxSensor[1])));

  /* USER CODE END LoRaTimerCallback */
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


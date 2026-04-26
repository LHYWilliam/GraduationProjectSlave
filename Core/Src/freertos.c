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
/* Definitions for LoRaRetryTask */
osThreadId_t LoRaRetryTaskHandle;
const osThreadAttr_t LoRaRetryTask_attributes = {
    .name = "LoRaRetryTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LEDTimer */
osTimerId_t LEDTimerHandle;
const osTimerAttr_t LEDTimer_attributes = {.name = "LEDTimer"};
/* Definitions for LoRaTimer */
osTimerId_t LoRaTimerHandle;
const osTimerAttr_t LoRaTimer_attributes = {.name = "LoRaTimer"};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void SlaveHeartbeat_Handler(void);
void SlaveUpload_Handler(void);

/* USER CODE END FunctionPrototypes */

void OLEDFlushTaskCode(void *argument);
void OLEDInteractionTaskCode(void *argument);
void LoRaRetryTaskCode(void *argument);
void LEDTimerCallback(void *argument);
void LoRaTimerCallback(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
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
  osTimerStart(LoRaTimerHandle, 10);

  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of OLEDFlushTask */
  OLEDFlushTaskHandle = osThreadNew(OLEDFlushTaskCode, NULL, &OLEDFlushTask_attributes);

  /* creation of OLEDInteraction */
  OLEDInteractionHandle = osThreadNew(OLEDInteractionTaskCode, NULL, &OLEDInteraction_attributes);

  /* creation of LoRaRetryTask */
  LoRaRetryTaskHandle = osThreadNew(LoRaRetryTaskCode, NULL, &LoRaRetryTask_attributes);

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

/* USER CODE BEGIN Header_LoRaRetryTaskCode */
/**
* @brief Function implementing the LoRaRetryTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LoRaRetryTaskCode */
void LoRaRetryTaskCode(void *argument)
{
  /* USER CODE BEGIN LoRaRetryTaskCode */
  /* Infinite loop */

  UNUSED(argument);

  for (;;)
  {
    if (Controller.Connecting && Controller.Register == 0)
    {
      uint8_t Pack[8] = {
          0xAA, SlaveRegister, 0x00, 0x04, Uint32ToUint8s(Controller.UID),
      };
      LoRa_SendPack(&LoRa, Pack, 8);
    }

    osDelay(1000);
  }
  /* USER CODE END LoRaRetryTaskCode */
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

  if (LoRa.ReceiveMessage)
  {
    if (Message.Type == SlaveRegisterAck)
    {
      if (Controller.Connecting)
      {
        uint32_t UID = Uint8ArrayToUint32(Message.Data);

        if (Controller.UID == UID)
        {
          Controller.ID = Message.Data[4];
          Controller.Register = 1;
          Controller.Connecting = 0;
        }
      }
    } else if (Message.Type == SlaveLogout)
    {
      uint8_t DeviceID = Message.Data[0];

      if (Controller.ID == DeviceID)
      {
        Controller.ID = 0;
        Controller.Upload = 0;
        Controller.Register = 0;
      }
    }

    LoRa_CLearReceive(&LoRa);
  }

  SlaveHeartbeat_Handler();

  SlaveUpload_Handler();

  /* USER CODE END LoRaTimerCallback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void SlaveHeartbeat_Handler(void)
{
  if (Controller.Register && osKernelGetTickCount() - Controller.LastHeartBeatTick > 5000)
  {
    Controller.LastHeartBeatTick = osKernelGetTickCount();

    uint8_t Pack[4] = {
        0xAA,
        SlaveHeartbeat,
        Controller.ID,
        0x00,
    };
    LoRa_SendPack(&LoRa, Pack, 4);
  }
}

void SlaveUpload_Handler(void)
{
  if (Controller.Register && Controller.Upload &&
      (osKernelGetTickCount() - Controller.LastUploadTick) > 1000)
  {
    Controller.LastUploadTick = osKernelGetTickCount();

    uint16_t Sensor1PPM = (uint16_t) MQSensor_CalculateMQ2PPM(MQSensor_GetData(&MQxSensor[0]));
    uint16_t Sensor2PPM = (uint16_t) MQSensor_CalculateMQ3PPM(MQSensor_GetData(&MQxSensor[1]));
    uint8_t Pack[8] = {
        0xAA,
        SlaveUpload,
        Controller.ID,
        0x04,
        Uint16ToUint8s(Sensor1PPM),
        Uint16ToUint8s(Sensor2PPM),
    };
    LoRa_SendPack(&LoRa, Pack, 8);
  }
}

/* USER CODE END Application */
